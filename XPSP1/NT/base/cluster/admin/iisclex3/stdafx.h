// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef _UNICODE
#define _UNICODE			// Microsoft Windows NT Cluster Administrator
							//   Extension DLLs need to be Unicode
							//   applications.
#endif

// Choose which threading model you want by commenting or uncommenting
// the proper constant definition.  If you want multi-threading
// (i.e. "both"), comment both definitions out.  Also change the
// THREADFLAGS_xxx set in the DECLARE_REGISTRY macro invokation in ExtObj.h
//#define _ATL_SINGLE_THREADED
#define _ATL_APARTMENT_THREADED

// Link against the Microsoft Windows NT Cluster API library.
#pragma comment(lib, "clusapi.lib")

// Link against the Cluster Administrator Extensions library.
#pragma comment(lib, "cluadmex.lib")

/////////////////////////////////////////////////////////////////////////////
// Common Pragmas
/////////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4100)		// unreferenced formal parameters
#pragma warning(disable : 4702)		// unreachable code
#pragma warning(disable : 4711)		// function selected for automatic inline expansion

/////////////////////////////////////////////////////////////////////////////
// Include Files
/////////////////////////////////////////////////////////////////////////////

#include <afxwin.h>			// MFC core and standard components
#include <afxext.h>			// MFC extensions
#include <afxdisp.h>
#include <afxtempl.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Need to include this for WM_COMMANDHELP.  Unfortunately, both afxpriv.h and
// atlconv.h define some of the same macros.  Since we are using ATL, we'll use
// the ATL versions.
#define __AFXCONV_H__
#include <afxpriv.h>
#undef __AFXCONV_H__
#undef DEVMODEW2A
#undef DEVMODEA2W
#undef TEXTMETRICW2A
#undef TEXTMETRICA2W

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#ifndef _CLUSTER_API_
#include <clusapi.h>	// for cluster definitions
#endif

/////////////////////////////////////////////////////////////////////////////
// Common Types
/////////////////////////////////////////////////////////////////////////////

typedef UINT	IDS;
typedef UINT	IDD;
