////////////////////////////////////////////////////////////////////////
//
//  FMStocks_HiPerfProvider.cpp
//
//	Module:	WMI high performance provider for F&M Stocks
//
//  Generic COM server framework, 
//
//  History:
//	a-vkanna      3-April-2000		Initial Version
//
//  Copyright (c) 2000 Microsoft Corporation
//
////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <initguid.h>

#include "ClassFactory.h"
//#include "..\..\unitrace\unitrace.h"

/////////////////////////////////////////////////////////////////////////////
//
//  BEGIN CLSID SPECIFIC SECTION
//
//

#include "wbemprov.h"
#include "HiPerfProvider.h"

// {7EA4A291-AD45-4882-80AD-267DA859CE39}
DEFINE_GUID(CLSID_FMStocks_HiPerfProvider, 
0x7ea4a291, 0xad45, 0x4882, 0x80, 0xad, 0x26, 0x7d, 0xa8, 0x59, 0xce, 0x39);

#define IMPLEMENTED_CLSID           CLSID_FMStocks_HiPerfProvider
#define SERVER_REGISTRY_COMMENT     L"FMStocks Lookup Performance Counter"

HINSTANCE g_hInstance;
long g_lLocks = 0;
long g_lObjects = 0;

//***************************************************************************
//
//  DllMain
//
//  Dll entry point.
//
//  PARAMETERS:
//
//      HINSTANCE hinstDLL      The handle to our DLL.
//      DWORD dwReason          DLL_PROCESS_ATTACH on load,
//                              DLL_PROCESS_DETACH on shutdown,
//                              DLL_THREAD_ATTACH/DLL_THREAD_DETACH otherwise.
//      LPVOID lpReserved       Reserved
//
//  RETURN VALUES:
//
//      TRUE is successful, FALSE if a fatal error occured.
//      NT behaves very ugly if FALSE is returned.
//
//***************************************************************************
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD dwReason,
    LPVOID lpReserved
    )
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        setlocale(LC_ALL, "");      // Set to the 'current' locale
        g_hInstance = hinstDLL;
    }

//	SetModuleName(_T("HIPERF"));
    return TRUE;
}

//***************************************************************************
//
//  DllGetClassObject
//
//  Standard OLE In-Process Server entry point to return an class factory
//  instance.
//
//  PARAMETERS:
//
//  RETURNS:
//
//      S_OK                Success
//      E_NOINTERFACE       An interface other that IClassFactory was asked for
//      E_OUTOFMEMORY
//      E_FAILED            Initialization failed, or an unsupported clsid was
//                          asked for.
//
//***************************************************************************

STDAPI DllGetClassObject(
    REFCLSID rclsid,
    REFIID riid,
    LPVOID * ppv
    )
{
    CClassFactory *pClassFactory = NULL;
	HRESULT hRes;

    //  Verify the caller is asking for our type of object
    if (IMPLEMENTED_CLSID == rclsid) 
	{
		// Create the class factory
		pClassFactory = new CClassFactory;

		if (!pClassFactory)
			return E_OUTOFMEMORY;
		
		hRes = pClassFactory->QueryInterface(riid, ppv);
		if (FAILED(hRes))
		{
			delete pClassFactory;
			return hRes;
		}
		hRes = S_OK;
	}
	else 
		hRes = CLASS_E_CLASSNOTAVAILABLE;

    return hRes;
}

//***************************************************************************
//
//  DllCanUnloadNow
//
//  Standard OLE entry point for server shutdown request. Allows shutdown
//  only if no outstanding objects or locks are present.
//
//  RETURN VALUES:
//
//      S_OK        May unload now.
//      S_FALSE     May not.
//
//***************************************************************************

STDAPI DllCanUnloadNow(void)
{
    HRESULT hRes = S_FALSE;

    if (0 == g_lLocks && 0 == g_lObjects)
        hRes = S_OK;

    return hRes;
}

//***************************************************************************
//
//  DllRegisterServer
//
//  Standard OLE entry point for registering the server.
//
//  RETURN VALUES:
//
//      S_OK        Registration was successful
//      E_FAIL      Registration failed.
//
//***************************************************************************

STDAPI DllRegisterServer(void)
{
    wchar_t Path[1024];
    wchar_t *pGuidStr = 0;
    wchar_t KeyPath[1024];

    // Get the dll's filename
    GetModuleFileNameW(g_hInstance, Path, 1024);

    // Convert CLSID to string.
    StringFromCLSID(IMPLEMENTED_CLSID, &pGuidStr);
    swprintf(KeyPath, L"Software\\Classes\\CLSID\\\\%s", pGuidStr);

    // Place it in registry.
    // CLSID\\CLSID_Nt5PerProvider_v1 : <no_name> : "name"
    //      \\CLSID_Nt5PerProvider_v1\\InProcServer32 : <no_name> : "path to DLL"
    //                                    : ThreadingModel : "both"
    HKEY hKey;
    LONG lRes = RegCreateKeyW(HKEY_LOCAL_MACHINE, KeyPath, &hKey);
    if (lRes)
        return E_FAIL;

    wchar_t *pName = SERVER_REGISTRY_COMMENT; 
    RegSetValueExW(hKey, 0, 0, REG_SZ, (const BYTE *) pName, wcslen(pName) * 2 + 2);

    HKEY hSubkey;
    lRes = RegCreateKey(hKey, _T("InprocServer32"), &hSubkey);

    RegSetValueExW(hSubkey, 0, 0, REG_SZ, (const BYTE *) Path, wcslen(Path) * 2 + 2);
    RegSetValueExW(hSubkey, L"ThreadingModel", 0, REG_SZ, (const BYTE *) L"Both", wcslen(L"Both") * 2 + 2);

    RegCloseKey(hSubkey);
    RegCloseKey(hKey);

    CoTaskMemFree(pGuidStr);

    return S_OK;
}

//***************************************************************************
//
//  DllUnregisterServer
//
//  Standard OLE entry point for unregistering the server.
//
//  RETURN VALUES:
//
//      S_OK        Unregistration was successful
//      E_FAIL      Unregistration failed.
//
//***************************************************************************

STDAPI DllUnregisterServer(void)
{
    wchar_t *pGuidStr = 0;
    HKEY hKey;
    wchar_t KeyPath[256];

    StringFromCLSID(IMPLEMENTED_CLSID, &pGuidStr);
    swprintf(KeyPath, L"Software\\Classes\\CLSID\\\\%s", pGuidStr);

    // Delete InProcServer32 subkey.
    LONG lRes = RegOpenKeyW(HKEY_LOCAL_MACHINE, KeyPath, &hKey);
    if (lRes)
        return E_FAIL;

    RegDeleteKeyW(hKey, L"InprocServer32");
    RegCloseKey(hKey);

    // Delete CLSID GUID key.
    lRes = RegOpenKeyW(HKEY_LOCAL_MACHINE, L"Software\\Classes\\CLSID", &hKey);
    if (lRes)
        return E_FAIL;

    RegDeleteKeyW(hKey, pGuidStr);
    RegCloseKey(hKey);

    CoTaskMemFree(pGuidStr);

    return S_OK;
}
