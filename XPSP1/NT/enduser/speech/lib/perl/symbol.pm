package Symbol;

=head1 NAME

Symbol - manipulate Perl symbols and their names

=head1 SYNOPSIS

    use Symbol;

    $sym = gensym;
    open($sym, "filename");
    $_ = <$sym>;
    # etc.

    ungensym $sym;      # no effect

    print qualify("x"), "\n";              # "Test::x"
    print qualify("x", "FOO"), "\n"        # "FOO::x"
    print qualify("BAR::x"), "\n";         # "BAR::x"
    print qualify("BAR::x", "FOO"), "\n";  # "BAR::x"
    print qualify("STDOUT", "FOO"), "\n";  # "main::STDOUT" (global)
    print qualify(\*x), "\n";              # returns \*x
    print qualify(\*x, "FOO"), "\n";       # returns \*x

    use strict refs;
    print { qualify_to_ref $fh } "foo!\n";
    $ref = qualify_to_ref $name, $pkg;

    use Symbol qw(delete_package);
    delete_package('Foo::Bar');
    print "deleted\n" unless exists $Foo::{'Bar::'};


=head1 DESCRIPTION

C<Symbol::gensym> creates an anonymous glob and returns a reference
to it.  Such a glob reference can be used as a file or directory
handle.

For backward compatibility with older implementations that didn't
support anonymous globs, C<Symbol::ungensym> is also provided.
But it doesn't do anything.

C<Symbol::qualify> turns unqualified symbol names into qualified
variable names (e.g. "myvar" -E<gt> "MyPackage::myvar").  If it is given a
second parameter, C<qualify> uses it as the default package;
otherwise, it uses the package of its caller.  Regardless, global
variable names (e.g. "STDOUT", "ENV", "SIG") are always qualfied with
"main::".

Qualification applies only to symbol names (strings).  References are
left unchanged under the assumption that they are glob references,
which are qualified by their nature.

C<Symbol::qualify_to_ref> is just like C<Symbol::qualify> except that it
returns a glob ref rather than a symbol name, so you can use the result
even if C<use strict 'refs'> is in effect.

C<Symbol::delete_package> wipes out a whole package namespace.  Note
this routine is not exported by default--you may want to import it
explicitly.

=cut

BEGIN { require 5.002; }

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(gensym ungensym qualify qualify_to_ref);
@EXPORT_OK = qw(delete_package);

$VERSION = 1.02;

my $genpkg = "Symbol::";
my $genseq = 0;

my %global = map {$_ => 1} qw(ARGV ARGVOUT ENV INC SIG STDERR STDIN STDOUT);

#
# Note that we never _copy_ the glob; we just make a ref to it.
# If we did copy it, then SVf_FAKE would be set on the copy, and
# glob-specific behaviors (e.g. C<*$ref = \&func>) wouldn't work.
#
sub gensym () {
    my $name = "GEN" . $genseq++;
    my $ref = \*{$genpkg . $name};
    delete $$genpkg{$name};
    $ref;
}

sub ungensym ($) {}

sub qualify ($;$) {
    my ($name) = @_;
    if (!ref($name) && index($name, '::') == -1 && index($name, "'") == -1) {
	my $pkg;
	# Global names: special character, "^x", or other. 
	if ($name =~ /^([^a-z])|(\^[a-z])$/i || $global{$name}) {
	    $pkg = "main";
	}
	else {
	    $pkg = (@_ > 1) ? $_[1] : caller;
	}
	$name = $pkg . "::" . $name;
    }
    $name;
}

sub qualify_to_ref ($;$) {
    return \*{ qualify $_[0], @_ > 1 ? $_[1] : caller };
}

#
# of Safe.pm lineage
#
sub delete_package ($) {
    my $pkg = shift;

    # expand to full symbol table name if needed

    unless ($pkg =~ /^main::.*::$/) {
        $pkg = "main$pkg"	if	$pkg =~ /^::/;
        $pkg = "main::$pkg"	unless	$pkg =~ /^main::/;
        $pkg .= '::'		unless	$pkg =~ /::$/;
    }

    my($stem, $leaf) = $pkg =~ m/(.*::)(\w+::)$/;
    my $stem_symtab = *{$stem}{HASH};
    return unless defined $stem_symtab and exists $stem_symtab->{$leaf};

    my $leaf_glob   = $stem_symtab->{$leaf};
    my $leaf_symtab = *{$leaf_glob}{HASH};

    %$leaf_symtab = ();
    delete $stem_symtab->{$leaf};
}

1;
