                   Permission Checker Component
                   ============================


Table of Contents
=================

    Overview
    Installation
    File List
    Samples
    Build Notes
    Support
    Change Notes


Overview
========

The Permission Checker Component is an Active Server Pages
component that allows to check the read access right for the
context user to given file. It can be used to exclude the items
that are not accessible to the current user from the generated
HTML page.


Installation
============

In order to use this component you must register it.  This will allow Active
Server Pages (ASP) Scripting Languages and other languages to make use of
the component.  ASP uses either the Server.CreateObject("ObjectName") syntax
or the <object id="myName" progid="ObjectName" runat="server"> syntax to
gain access to an object.  New objects can be made ready for use by
installing a new component.  Note: One component may contain more than one
object definition.

The following directions are to help you register the component for use:
 1. Use the Start menu, Programs option to start a Command Prompt
 2. Type the following:
        cd \InetPub\ASPSamp\Components\PermChk\DLL\i386
 3. Type:
        regsvr32 PermChk.dll
Note: you must register the component on each IIS server where you intend
to use it.

If you have trouble registering components, you may be using the wrong
version of RegSvr32.exe.  Please use the version installed by default in
the directory <InstallDir>\ASP\Cmpnts.  On Windows NT, the default
installation directory is \winnt\System32\Inetsrv.  On Windows 95, it is
\Program Files\WebSvr\System.

(If you rebuild the source code, the makefile will automatically reregister
the component for you.)

The following directions are to help you test the registered component:
 1. Use the Windows Explorer to copy all of the Sample files from
    \InetPub\ASPSamp\Components\PermChk\Samples to \InetPub\ASPSamp\Samples.
 2. In your browser, open http://localhost/ASPSamp/Samples/PermChk.asp
You must copy the sample files to a virtual directory; if you attempt to
examine them with a browser in the PermChk\Samples directory, ASP will not
execute the script.

Note that the user is Anonymous unless Permissions for Everyone and
System are removed from the file PermChk.asp.  Testing PermChk.asp with
individual access rights also requires Basic (Clear Text) or Windows NT
Challenge/Response Password Authentication to be enabled.  A further
discussion on this topic can be found in the Docs directory for this
component.


File List
=========

File             Description
----             -----------

.\Source

 PermChk.cpp     the C++ source code for the permission checker component
 PermChk.h       declarations for CPermissionChecker
 PermChk.idl     declaration of IPermissionChecker, the IDispatch-based interface
 Makefile        a makefile that can be used with nmake
 PermChk.mak     the Developer Studio makefile
 util.cpp        utility functions
 util.h          utility function prototypes
 PermChk.cpp     )
 PermChk.def     )
 PermChk.mdp     )
 PermChk.rc      )
 PrmChkPS.def    } Generated by the ATL COM AppWizard
 PrmChkPS.mak    )
 Resource.h      )
 StdAfx.cpp      )
 StdAfx.h        )

.\Samples

 PermChk.asp     sample ASP that checks and displays file permission
 a.htm           sample data file
 b.txt           sample data file
 c.doc           sample data file


Samples
=======

You will need to copy the sample files to a virtual directory on
an IIS Server (it need not be a virtual root).  To run the test,
you should change the NTFS permissions on a.htm, b.txt, and
c.doc using NT Explorer / Properties / Security / Permissions.


Build Notes
===========

This sample requires Microsoft Visual C++ 4.2b or newer.  If you are using
VC 4.2, it is necessary that you upgrade to VC 4.2b, using the patch which
can be found at http://www.microsoft.com/visualc/patches/v4.2b/vc42b.htm
Note that this patch will not work with earlier or later versions of
Visual C++, only with VC 4.2.

This sample also requires ATL (Microsoft Active Template Library)
version 2.0 or newer.  ATL 2.1 ships with Visual C++ 5.0.  ATL 2.0 for
VC 4.2b can be downloaded from: http://www.microsoft.com/visualc/prodinfo/
You do not need the ATL Docs or Object Wizard Technology Preview to build
the registry access component, but you will probably find them useful.

If you get an error about "don't know how to make asptlb.h", you will
need to copy <InstallDir>\ASP\Cmpnts\AspTlb.h to your include
directory.

You can build this component with nmake at the command line.  Read
Makefile for more details.  You can also build it in Microsoft
Developer Studio, using the PermChk.mdp project.

The component can be built as ANSI or Unicode.  If you intend to
run it on Windows 95, build it as ANSI.


Support
=======

This component is not officially supported by Microsoft Corporation.
Peer support is available on the Active Server Pages mailing list or on
the microsoft.public.inetserver.iis.activeserverpages newsgroup.

To subscribe to the Active Server Pages mailing list, send mail to
listserv@listserv.msn.com with

subscribe Denali [firstname lastname]

in the body of the message, and then follow the directions carefully.
(firstname and lastname are optional.)

You can reach the newsgroup through msnews.microsoft.com and other NNTP
servers.


Change Notes
============

Beta 1: February 1997
---------------------

First release.


Beta 2: March 1997
------------------

* Fixed Developer Studio makefile problems in C++ components.
* Upgraded to build cleanly with ATL 2.0 (Visual C++ 4.2b) and ATL 2.0 (VC5).
* Type Library name changes
