# NOTE: Derived from ..\..\lib\POSIX.pm.
# Changes made here will be lost when autosplit again.
# See AutoSplit.pm.
package POSIX;

#line 566 "..\..\lib\POSIX.pm (autosplit into ..\..\lib\auto/POSIX/abs.al)"
sub abs {
    usage "abs(x)" if @_ != 1;
    CORE::abs($_[0]);
}

# end of POSIX::abs
1;
