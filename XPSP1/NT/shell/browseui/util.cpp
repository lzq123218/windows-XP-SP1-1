#include "priv.h"
#include "sccls.h"
#include "mlang.h"  // fo char conversion
#include "bandprxy.h"
#include "resource.h"
#include <shdocvw.h>
#include <icwcfg.h>
#include <advpub.h> // for IE activesetup GUID
#include <shellapi.h>
#include "apithk.h" //for WM_KEYBOARDCUES msg
#include <platform.h>
#include <mobsync.h>
#include <mobsyncp.h>

#include "..\shell32\shitemid.h"    // for SHID_XX

#ifdef UNIX
#include "unixstuff.h"
#endif /* UNIX */

#define MLUI_INIT
#include "mluisupp.h"

//small (previously duplicated) functions shared between shdcovw and browseui
#include "..\inc\brutil.cpp"

// #define MLUI_SUPPORT   1

LCID g_lcidLocale = MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT);

#define DM_NAV              TF_SHDNAVIGATE
#define DM_ZONE             TF_SHDNAVIGATE
#define DM_IEDDE            DM_TRACE
#define DM_CANCELMODE       0
#define DM_UIWINDOW         0
#define DM_ENABLEMODELESS   0
#define DM_EXPLORERMENU     0
#define DM_BACKFORWARD      0
#define DM_PROTOCOL         0
#define DM_ITBAR            0
#define DM_STARTUP          0
#define DM_AUTOLIFE         0
#define DM_PALETTE          0

const VARIANT c_vaEmpty = {0};
const LARGE_INTEGER c_li0 = { 0, 0 };

#ifdef DEBUG
const DWORD TempBuffer::s_dummy = 0;
#endif



#undef VariantCopy

WINOLEAUTAPI VariantCopyLazy(VARIANTARG * pvargDest, VARIANTARG * pvargSrc)
{
    VariantClearLazy(pvargDest);

    switch(pvargSrc->vt)
    {
    case VT_I4:
    case VT_UI4:
    case VT_BOOL:
        // we can add more
        *pvargDest = *pvargSrc;
        return S_OK;

    case VT_UNKNOWN:
        if (pvargDest)
        {
            *pvargDest = *pvargSrc;
            pvargDest->punkVal->AddRef();
            return S_OK;
        }
        ASSERT(0);
        return E_INVALIDARG;
    }

    return VariantCopy(pvargDest, pvargSrc);
}

//
// WARNING: This function must be placed at the end because we #undef
// VariantClear
//
#undef VariantClear

HRESULT VariantClearLazy(VARIANTARG *pvarg)
{
    switch(pvarg->vt)
    {
    case VT_I4:
    case VT_UI4:
    case VT_EMPTY:
    case VT_BOOL:
        // No operation
        break;

    default:
        return VariantClear(pvarg);
    }
    return S_OK;
}


HRESULT QueryService_SID_IBandProxy(IUnknown * punkParent, REFIID riid, IBandProxy ** ppbp, void **ppvObj)
{
    HRESULT hr = E_FAIL;

    if (ppbp)
    {
        if (NULL == (*ppbp))
            hr = IUnknown_QueryService(punkParent, SID_IBandProxy, IID_PPV_ARG(IBandProxy, ppbp));

        if (*ppbp && ppvObj)
            hr = (*ppbp)->QueryInterface(riid, ppvObj);        // They already have the object.
    }


    return hr;
}

HRESULT CreateIBandProxyAndSetSite(IUnknown * punkParent, REFIID riid, IBandProxy ** ppbp, void **ppvObj)
{
    ASSERT(ppbp);

    HRESULT hr = CoCreateInstance(CLSID_BandProxy, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARG(IBandProxy, ppbp));
    if (SUCCEEDED(hr))
    {
        // Set the site
        ASSERT(*ppbp);
        (*ppbp)->SetSite(punkParent);

        if (ppvObj)
            hr = (*ppbp)->QueryInterface(riid, ppvObj);
    }
    else
    {
        if (ppvObj)
            *ppvObj = NULL;
    }
    return hr;
}

HRESULT IUnknown_FileSysChange(IUnknown* punk, DWORD dwEvent, LPCITEMIDLIST* ppidl)
{
    HRESULT hres = E_FAIL;
    if (punk)
    {
        IAddressBand * pab;
        hres = punk->QueryInterface(IID_PPV_ARG(IAddressBand, &pab));
        if (SUCCEEDED(hres))
        {
            hres = pab->FileSysChange(dwEvent, ppidl);
            pab->Release();
        }
    }
    return hres;
}


UINT    g_cfURL = 0;
UINT    g_cfHIDA = 0;
UINT    g_cfFileDescA = 0;
UINT    g_cfFileDescW = 0;
UINT    g_cfFileContents = 0;
UINT    g_cfPreferedEffect = 0;

void InitClipboardFormats()
{
    if (g_cfURL == 0)
    {
        g_cfURL = RegisterClipboardFormat(CFSTR_SHELLURL);
        g_cfHIDA = RegisterClipboardFormat(CFSTR_SHELLIDLIST);
        g_cfFileDescA = RegisterClipboardFormat(CFSTR_FILEDESCRIPTORA);
        g_cfFileDescW = RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW);
        g_cfFileContents = RegisterClipboardFormat(CFSTR_FILECONTENTS);
        g_cfPreferedEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
    }
}

DEFFOLDERSETTINGS g_dfs = INIT_DEFFOLDERSETTINGS;

void _InitDefaultFolderSettings()
{
    if (GetSystemMetrics(SM_CLEANBOOT))
        return;

    g_dfs.vid = g_bRunOnNT5 ? VID_LargeIcons : DFS_VID_Default;

    DEFFOLDERSETTINGS dfs = g_dfs;
    DWORD dwType, cbData = sizeof(dfs);

    if (SUCCEEDED(SKGetValue(SHELLKEY_HKCU_EXPLORER, REGVALUE_STREAMS, TEXT("Settings"), &dwType, &dfs, &cbData)) 
    && dwType == REG_BINARY)
    {
        if (cbData < sizeof(DEFFOLDERSETTINGS_W2K) || dfs.dwStructVersion < DFS_NASH_VER)
        {
            dfs.vid = g_bRunOnNT5 ? VID_LargeIcons : DFS_VID_Default;
            dfs.dwStructVersion = DFS_NASH_VER;
            dfs.bUseVID = TRUE;
        }

        if (cbData < sizeof(DEFFOLDERSETTINGS) || dfs.dwStructVersion < DFS_WHISTLER_VER)
        {
            dfs.dwViewPriority = VIEW_PRIORITY_CACHEMISS;
            dfs.dwStructVersion = DFS_WHISTLER_VER;
        }

        g_dfs = dfs;
    }
}

CABINETSTATE g_CabState = { 0 };
extern HANDLE g_hCabStateChange;
LONG g_lCabStateCount = -1;     // never a valid count

void GetCabState(CABINETSTATE *pcs)
{
    if (g_hCabStateChange == NULL)
        g_hCabStateChange = SHGlobalCounterCreate(GUID_FolderSettingsChange);

    LONG lCabStateCur = SHGlobalCounterGetValue(g_hCabStateChange);
    if (g_lCabStateCount != lCabStateCur)
    {
        g_lCabStateCount = lCabStateCur;
        if (!ReadCabinetState(&g_CabState, sizeof(g_CabState)))
        {
            WriteCabinetState(&g_CabState);
        }
    }
    *pcs = g_CabState;
}

typedef struct tagINIPAIR
{
    DWORD dwFlags;
    LPCTSTR pszSection;
}
INIPAIR, *PINIPAIR;

const INIPAIR c_aIniPairs[] =
{
    EICH_KINET,          TEXT("Software\\Microsoft\\Internet Explorer"),
    EICH_KINETMAIN,      TEXT("Software\\Microsoft\\Internet Explorer\\Main"),
    EICH_KWIN,           TEXT("Software\\Microsoft\\Windows\\CurrentVersion"),
    EICH_KWINEXPLORER,   TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"),
    EICH_KWINEXPLSMICO,  TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SmallIcons"),
    EICH_KWINPOLICY,     TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"),
    EICH_SSAVETASKBAR,   TEXT("SaveTaskbar"),
    EICH_SWINDOWMETRICS, TEXT("WindowMetrics"),
    EICH_SSHELLMENU,     TEXT("ShellMenu"),
    EICH_SPOLICY,        TEXT("Policy"),
    EICH_SWINDOWS,       TEXT("Windows"),
};

DWORD SHIsExplorerIniChange(WPARAM wParam, LPARAM lParam)
{
    DWORD dwFlags = 0;

    if (lParam == 0)
    {
        if (wParam == 0)
        {
            dwFlags = EICH_UNKNOWN;
        }
    }
    else
    {
        //
        // In the wacky world of browseui, UNICODE-ANSI doesn't vary from
        // window to window.  Instead, on NT browseui registers all windows
        // UNICODE, while on 9x user browseui registers all windows ANSI.
        //
        USES_CONVERSION;
        LPCTSTR pszSection;

        if (g_fRunningOnNT)
            pszSection = W2CT((LPCWSTR)lParam);
        else
            pszSection = A2CT((LPCSTR)lParam);

        for (int i = 0; !dwFlags && i < ARRAYSIZE(c_aIniPairs); i++)
        {
            if (StrCmpI(pszSection, c_aIniPairs[i].pszSection) == 0)
            {
                dwFlags = c_aIniPairs[i].dwFlags;
            }
        }
    }

    return dwFlags;
}

void _InitAppGlobals()
{
    static BOOL fInitialized = FALSE;
    if (!fInitialized)
    {
        _InitComCtl32();
        _InitDefaultFolderSettings();

        // dont put anything else here. instead init on demand

        fInitialized = TRUE;        // allow a race on the above calls
    }
}

BOOL _InitComCtl32()
{
    static BOOL fInitialized = FALSE;
    if (!fInitialized)
    {
        INITCOMMONCONTROLSEX icc;

        icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icc.dwICC = ICC_USEREX_CLASSES | ICC_COOL_CLASSES | ICC_INTERNET_CLASSES | ICC_PAGESCROLLER_CLASS
            | ICC_NATIVEFNTCTL_CLASS;
        fInitialized = InitCommonControlsEx(&icc);
    }
    return fInitialized;
}


DWORD GetPreferedDropEffect(IDataObject *pdtobj)
{
    InitClipboardFormats();

    DWORD dwEffect = 0;
    STGMEDIUM medium;
    DWORD *pdw = (DWORD *)DataObj_GetDataOfType(pdtobj, g_cfPreferedEffect, &medium);
    if (pdw)
    {
        dwEffect = *pdw;
        ReleaseStgMediumHGLOBAL(pdw,&medium);
    }
    return dwEffect;
}

HRESULT _SetPreferedDropEffect(IDataObject *pdtobj, DWORD dwEffect)
{
    InitClipboardFormats();

    HRESULT hres = E_OUTOFMEMORY;
    DWORD *pdw = (DWORD *)GlobalAlloc(GPTR, sizeof(*pdw));
    if (pdw)
    {
        STGMEDIUM medium;
        FORMATETC fmte = {g_cfPreferedEffect, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        *pdw = dwEffect;

        medium.tymed = TYMED_HGLOBAL;
        medium.hGlobal = pdw;
        medium.pUnkForRelease = NULL;

        hres = pdtobj->SetData(&fmte, &medium, TRUE);

        if (FAILED(hres))
            GlobalFree((HGLOBAL)pdw);
    }
    return hres;
}

//***   Reg_GetStrs -- get values from registry, assign to struct
void Reg_GetStrs(HKEY hkey, const struct regstrs *tab, LPTSTR szBuf, int cchBuf, void *pv)
{
    for (; tab->name != NULL; tab++)
    {
        ULONG cbTmp = cchBuf;

        // NOTE: IE4 did *NOT* support SHLoadRegUIString, so don't call Reg_GetStrs
        // on roamable data.  (Or at least don't register plugui strings there.)
        if (ERROR_SUCCESS == SHLoadRegUIString(hkey, tab->name, szBuf, cbTmp))
        {
            // pv->field = StrDup(szBuf)
            *(LPTSTR *)((char *)pv + tab->off) = StrDup(szBuf);
        }
    }
    return;
}

BOOL g_fNewNotify = FALSE;   // Are we using classic mode (W95 or new mode?
PFNSHCHANGENOTIFYREGISTER    g_pfnSHChangeNotifyRegister = NULL;
PFNSHCHANGENOTIFYDEREGISTER  g_pfnSHChangeNotifyDeregister = NULL;

#define GET_PRIVATE_PROC_ADDRESS(_hinst, _fname, _ord) GetProcAddress(_hinst, _ord)

BOOL _DelayLoadRegisterNotify(void)
{
    // See if we need to still figure out which version of SHChange Notify to call?
    if  (g_pfnSHChangeNotifyDeregister == NULL)
    {
        // This should never fail, since we are load-time-linked to SHELL32
        HMODULE hmodShell32 = GetModuleHandleA("SHELL32.DLL");
        if (hmodShell32)
        {
            g_pfnSHChangeNotifyRegister = (PFNSHCHANGENOTIFYREGISTER)GET_PRIVATE_PROC_ADDRESS(hmodShell32, "NTSHChangeNotifyRegister",MAKEINTRESOURCEA(640));
            if (g_pfnSHChangeNotifyRegister && (WhichPlatform() == PLATFORM_INTEGRATED))
            {
                g_pfnSHChangeNotifyDeregister = (PFNSHCHANGENOTIFYDEREGISTER)GET_PRIVATE_PROC_ADDRESS(hmodShell32,"NTSHChangeNotifyDeregister", MAKEINTRESOURCEA(641));
                g_fNewNotify = TRUE;
            }
            else
            {
                g_pfnSHChangeNotifyRegister = (PFNSHCHANGENOTIFYREGISTER)GET_PRIVATE_PROC_ADDRESS(hmodShell32, "SHChangeNotifyRegister", MAKEINTRESOURCEA(2));
                g_pfnSHChangeNotifyDeregister = (PFNSHCHANGENOTIFYDEREGISTER)GET_PRIVATE_PROC_ADDRESS(hmodShell32, "SHChangeNotifyDeregister",MAKEINTRESOURCEA(4));
            }
        }
    }
    return (NULL == g_pfnSHChangeNotifyDeregister) ? FALSE : TRUE;
}

ULONG RegisterNotify(HWND hwnd, UINT nMsg, LPCITEMIDLIST pidl, DWORD dwEvents, UINT uFlags, BOOL fRecursive)
{
    if (_DelayLoadRegisterNotify())
    {
        SHChangeNotifyEntry fsne;

        if (g_fNewNotify)
            uFlags |= SHCNRF_NewDelivery;

        fsne.fRecursive = fRecursive;
        fsne.pidl = pidl;
        return g_pfnSHChangeNotifyRegister(hwnd, uFlags, dwEvents, nMsg, 1, &fsne);
    }
    return 0;
}

int PropBag_ReadInt4(IPropertyBag* pPropBag, LPWSTR pszKey, int iDefault)
{
    SHPropertyBag_ReadInt(pPropBag, pszKey, &iDefault);
    return iDefault;
}


STDAPI_(BOOL) _EnsureLoaded(HINSTANCE *phinst, LPCSTR pszDLL)
{
    if (*phinst == NULL)
    {
#ifdef DEBUG
        if (g_dwDumpFlags & DF_DELAYLOADDLL)
        {
            TraceMsg(TF_ALWAYS, "DLLLOAD: Loading %s for the first time", pszDLL);
        }

        if (g_dwBreakFlags & 0x00000080)
        {
            DebugBreak();
        }
#endif
        *phinst = LoadLibraryA(pszDLL);
        if (*phinst == NULL)
        {
            return FALSE;
        }
    }
    return TRUE;
}


// global g_hinst values 
HINSTANCE g_hinstSHDOCVW = NULL;
HINSTANCE g_hinstShell32 = NULL;

HINSTANCE HinstShdocvw()
{
    _EnsureLoaded(&g_hinstSHDOCVW, "shdocvw.dll");
    return g_hinstSHDOCVW;
}

HINSTANCE HinstShell32()
{
    _EnsureLoaded(&g_hinstShell32, "shell32.dll");
    return g_hinstShell32;
}

STDAPI_(BOOL) CallCoInternetQueryInfo(LPCTSTR pszURL, QUERYOPTION QueryOption)
{
    DWORD fRetVal;
    DWORD dwSize;
    WCHAR wszURL[MAX_URL_STRING];

    SHTCharToUnicode(pszURL, wszURL, ARRAYSIZE(wszURL));
    return SUCCEEDED(CoInternetQueryInfo(
                        wszURL, QueryOption,
                        0, &fRetVal, sizeof(fRetVal), &dwSize, 0)) && fRetVal;
}


HRESULT IURLQualifyW(IN LPCWSTR pcwzURL, DWORD dwFlags, OUT LPWSTR pwzTranslatedURL, LPBOOL pbWasSearchURL, LPBOOL pbWasCorrected)
{
    return IURLQualify(pcwzURL, dwFlags, pwzTranslatedURL, pbWasSearchURL, pbWasCorrected);
}

BSTR LoadBSTR(HINSTANCE hinst, UINT uID)
{
    WCHAR wszBuf[128];
    if (LoadStringW(hinst, uID, wszBuf, ARRAYSIZE(wszBuf)))
    {
        return SysAllocString(wszBuf);
    }
    return NULL;
}

HRESULT _SetStdLocation(LPTSTR szPath, UINT id)
{
    HRESULT hres = E_FAIL;
    WCHAR szDefaultPath[MAX_URL_STRING];

    ASSERT(id == DVIDM_GOHOME);
    if (SUCCEEDED(URLSubLoadString(MLGetHinst(), IDS_DEF_HOME, szDefaultPath, SIZECHARS(szDefaultPath), URLSUB_ALL)))
    {
        if (!StrCmp(szDefaultPath, szPath))
            return S_OK;  // We don't need to write out the name string.
    }

    DWORD cbSize = (lstrlen(szPath) + 1) * sizeof(TCHAR);
    if (ERROR_SUCCESS == SHSetValue(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Internet Explorer\\Main"), (id==DVIDM_GOHOME) ? TEXT("Start Page") : TEXT("Search Page"),
                REG_SZ, (LPBYTE)szPath, cbSize))
    {
        hres = S_OK;
    }

    return hres;
}

//***   IsVK_TABCycler -- is key a TAB-equivalent
// ENTRY/EXIT
//  dir     0 if not a TAB, non-0 if a TAB
// NOTES
//  NYI: -1 for shift+tab, 1 for tab
//
int IsVK_TABCycler(MSG *pMsg)
{
    int nDir = 0;

    if (!pMsg)
        return nDir;

    if (pMsg->message != WM_KEYDOWN)
        return nDir;
    if (! (pMsg->wParam == VK_TAB || pMsg->wParam == VK_F6))
        return nDir;

    nDir = (GetKeyState(VK_SHIFT) < 0) ? -1 : 1;

#ifdef KEYBOARDCUES
    HWND hwndParent = GetParent(pMsg->hwnd);

    if (hwndParent)
        SendMessage(hwndParent, WM_CHANGEUISTATE, MAKEWPARAM(UIS_CLEAR, UISF_HIDEFOCUS), 0);
#endif
    return nDir ;
}

BOOL IsVK_CtlTABCycler(MSG *pMsg)
{
    if (IsVK_TABCycler(pMsg))
    {
        if (GetKeyState(VK_CONTROL) < 0 || (pMsg->wParam == VK_F6))
            return TRUE;
    }

    return FALSE;
}

const ITEMIDLIST s_idlNULL = { 0 } ;

// Copied from shell32 (was _ILCreate), which does not export this.
// The fsmenu code needs this function.
STDAPI_(LPITEMIDLIST) IEILCreate(UINT cbSize)
{
    LPITEMIDLIST pidl = (LPITEMIDLIST)SHAlloc(cbSize);
    if (pidl)
        memset(pidl, 0, cbSize);      // needed for external task allicator

    return pidl;
}

void SaveDefaultFolderSettings(UINT flags)
{
    ASSERT(!(flags & ~GFSS_VALID));

    if (flags & GFSS_SETASDEFAULT)
        g_dfs.dwDefRevCount++;

    SKSetValue(SHELLKEY_HKCU_EXPLORER, REGVALUE_STREAMS, TEXT("Settings"), REG_BINARY, &g_dfs, sizeof(g_dfs));
}

BOOL ViewIDFromViewMode(UINT uViewMode, SHELLVIEWID *pvid)
{
    switch (uViewMode)
    {
    case FVM_ICON:
        *pvid = VID_LargeIcons;
        break;

    case FVM_SMALLICON:
        *pvid = VID_SmallIcons;
        break;

    case FVM_LIST:
        *pvid = VID_List;
        break;

    case FVM_DETAILS:
        *pvid = VID_Details;
        break;

    case FVM_THUMBNAIL:
        *pvid = VID_Thumbnails;
        break;

    case FVM_TILE:
        *pvid = VID_Tile;
        break;

    default:
        *pvid = VID_LargeIcons;
        return(FALSE);
    }

    return(TRUE);
}

// This is a hack for IE6 23652 Beta 2. Remove in Whistler RC 1.
BOOL CheckForOutlookExpress()
{
    HKEY hKeyMail   = NULL;
    HKEY hKeyOE     = NULL;
    DWORD dwErr     = 0;
    DWORD dwSize    = 0;
    TCHAR szBuf[MAX_PATH];
    DWORD dwType    = 0;
    BOOL bRet = FALSE;

    // Open the key for default internet mail client
    // HKLM\Software\Clients\Mail

    dwErr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Clients\\Mail"), 0, KEY_READ, &hKeyMail);
    if(dwErr != ERROR_SUCCESS)
    {
        // DebugTrace( TEXT("RegopenKey %s Failed -> %u\n"), szDefMailKey, dwErr);
        goto out;
    }

    dwSize = ARRAYSIZE(szBuf);         // Expect ERROR_MORE_DATA
    dwErr = RegQueryValueEx(    hKeyMail, NULL, NULL, &dwType, (LPBYTE)szBuf, &dwSize);
    if(dwErr != ERROR_SUCCESS)
    {
        goto out;
    }

    if(!lstrcmpi(szBuf, TEXT("Outlook Express")))
    {
        // Yes its outlook express ..
        bRet = TRUE;
    }

out:
    if(hKeyOE)
        RegCloseKey(hKeyOE);
    if(hKeyMail)
        RegCloseKey(hKeyMail);
    return bRet;
}


HRESULT DropOnMailRecipient(IDataObject *pdtobj, DWORD grfKeyState)
{
    IDropTarget *pdrop;
    HRESULT hres = CoCreateInstance(CLSID_MailRecipient,
        NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        IID_PPV_ARG(IDropTarget, &pdrop));

    ULONG_PTR uCookie = 0;
    if (CheckForOutlookExpress())
    {
        SHActivateContext(&uCookie);
    }
    if (SUCCEEDED(hres))
    {
        hres = SHSimulateDrop(pdrop, pdtobj, grfKeyState, NULL, NULL);
        pdrop->Release();
    }
    if (uCookie)
    {
        SHDeactivateContext(uCookie);
    }

    return hres;
}



//
// This function cannot return Non -NULL pointers if
// it returns a FAILED(hr)
//

HRESULT CreateShortcutSetSiteAndGetDataObjectIfPIDLIsNetUrl(
    LPCITEMIDLIST pidl,
    IUnknown *pUnkSite,
    IUniformResourceLocator **ppUrlOut,
    IDataObject **ppdtobj
)
{
    HRESULT hr;
    TCHAR szUrl[MAX_URL_STRING];


    ASSERT(ppUrlOut);
    ASSERT(ppdtobj);
    *ppUrlOut = NULL;
    *ppdtobj = NULL;
    szUrl[0] = TEXT('\0');

    hr = IEGetNameAndFlags(pidl, SHGDN_FORPARSING, szUrl, SIZECHARS(szUrl), NULL);

    if ((S_OK == hr) && (*szUrl))
    {

       BOOL fIsHTML = FALSE;
       BOOL fHitsNet = UrlHitsNetW(szUrl);

       if (!fHitsNet)
       {
            if (URL_SCHEME_FILE == GetUrlScheme(szUrl))
            {
                TCHAR *szExt = PathFindExtension(szUrl);
                if (szExt)
                {
                    fIsHTML = ((0 == StrCmpNI(szExt, TEXT(".htm"),4)) ||
                              (0 == StrCmpNI(szExt, TEXT(".html"),5)));
                }
            }
       }

       if (fHitsNet || fIsHTML)
       {
            // Create a shortcut object and
            HRESULT hr = CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER,
                            IID_PPV_ARG(IUniformResourceLocator, ppUrlOut));
            if (SUCCEEDED(hr))
            {

                hr = (*ppUrlOut)->SetURL(szUrl, 0);
                if (S_OK == hr)
                {

                    // Get the IDataObject and send that back for the Drag Drop
                    hr = (*ppUrlOut)->QueryInterface(IID_PPV_ARG(IDataObject, ppdtobj));
                    if (SUCCEEDED(hr))
                    {
                        IUnknown_SetSite(*ppUrlOut, pUnkSite); // Only set the site if we're sure of
                                                          // returning SUCCESS
                    }
                }
           }
       }
       else
       {
            hr = E_FAIL;
       }
    }

    if (FAILED(hr))
    {
        SAFERELEASE(*ppUrlOut);
        SAFERELEASE(*ppdtobj);
    }
    return hr;
}

HRESULT SendDocToMailRecipient(LPCITEMIDLIST pidl, UINT uiCodePage, DWORD grfKeyState, IUnknown *pUnkSite)
{
    IDataObject *pdtobj = NULL;
    IUniformResourceLocator *purl = NULL;
    HRESULT hr = CreateShortcutSetSiteAndGetDataObjectIfPIDLIsNetUrl(pidl, pUnkSite, &purl, &pdtobj);
    if (FAILED(hr))
    {
        ASSERT(NULL == pdtobj);
        ASSERT(NULL == purl);
        hr = GetDataObjectForPidl(pidl, &pdtobj);
    }

    if (SUCCEEDED(hr))
    {
        IQueryCodePage * pQcp;
        if (SUCCEEDED(pdtobj->QueryInterface(IID_PPV_ARG(IQueryCodePage, &pQcp))))
        {
            pQcp->SetCodePage(uiCodePage);
            pQcp->Release();
        }
        hr = DropOnMailRecipient(pdtobj, grfKeyState);
        pdtobj->Release();
    }

    if (purl)
    {
        IUnknown_SetSite(purl, NULL);
        purl->Release();
    }
    return hr;
}

#ifdef DEBUG
/****************************************************\
    FUNCTION: Dbg_PidlStr

    DESCRIPTION:
        Create a display name for the pidl passed in
    and store the display name in pszBuffer.
\****************************************************/
LPTSTR Dbg_PidlStr(LPCITEMIDLIST pidl, LPTSTR pszBuffer, DWORD cchBufferSize)
{
    if (pidl)
    {
        if (ILIsRooted(pidl))
            StrCpyN(pszBuffer, TEXT("<ROOTED>"), cchBufferSize);
        else
        {
            IEGetNameAndFlags(pidl, SHGDN_FORPARSING, pszBuffer, cchBufferSize, NULL);
        }
    }
    else
        StrCpyN(pszBuffer, TEXT("<NULL>"), cchBufferSize);

    return pszBuffer;
}
#endif // DEBUG


#ifdef DEBUG
#define MAX_DEPTH 8

void Dbg_RecursiveDumpMenu(HMENU hmenu, int iDepth)
{
    if (!hmenu || iDepth > MAX_DEPTH)
        return;

    TCHAR szTabs[MAX_DEPTH + 1];
    for (int i = 0; i < iDepth; i++)
    {
        szTabs[i] = '\t';
    }
    szTabs[iDepth] = '\0';

    int cItems = GetMenuItemCount(hmenu);
    for (i = 0; i < cItems; i++)
    {
        MENUITEMINFO mii;
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_TYPE;

        TCHAR szTmp[64];
        mii.dwTypeData = szTmp;
        mii.cch = ARRAYSIZE(szTmp);

        if (GetMenuItemInfoWrap(hmenu, i, TRUE, &mii))
        {
            LPTSTR pszType;
            if (mii.fType == MFT_STRING && mii.dwTypeData)
                pszType = mii.dwTypeData;
            else
                pszType = TEXT("");

            TraceMsg(TF_ALWAYS, "%swID %x\tfType %x\t%s", szTabs, mii.wID, mii.fType, pszType);

            if (mii.hSubMenu)
            {
                Dbg_RecursiveDumpMenu(mii.hSubMenu, iDepth + 1);
            }
        }
    }
}

// FUNCTION: Dbg_DumpMenu
//
// walk hmenu & dump every item
void Dbg_DumpMenu(LPCTSTR psz, HMENU hmenu)
{
    if (IsFlagSet(g_dwDumpFlags, DF_DEBUGMENU))
    {
        TraceMsg(TF_ALWAYS, "Dumping hmenu %x (%s)", hmenu, psz);
        Dbg_RecursiveDumpMenu(hmenu, 0);
        TraceMsg(TF_ALWAYS, "End hmenu dump");
    }
}
#endif


// evil evil evil. for browse only mode support. not the right way to do things
STDAPI LookForDesktopIniText(IShellFolder *psf, LPCITEMIDLIST pidl, LPCTSTR pszKey, LPTSTR pszBuffer, DWORD cbSize);

#define CLSID_SIZE 40

HRESULT LoadHandler(const CLSID * pCLSID, LPCWSTR pszBuffer, REFIID riid, void **ppvObj)
{
    ASSERT(pszBuffer);
    CLSID clsid;

    if (!pCLSID)
    {
        // find the extension first ....
        // REARCHITECT - Shouldn't this be PathFindExtension?
        // Otherwise we will get confused by "foo.bar\baz"
        LPCWSTR pszDot = StrRChrW(pszBuffer, NULL, WCHAR('.'));
        if (!pszDot)
        {
            return E_NOINTERFACE;
        }

        HKEY hKey;
        USES_CONVERSION;
        LONG lRes = RegOpenKey(HKEY_CLASSES_ROOT, W2CT(pszDot), &hKey);
        if (lRes != ERROR_SUCCESS)
        {
            return E_NOINTERFACE;
        }

        TCHAR szSubKey[CLSID_SIZE + 10];
        StrCpy(szSubKey, TEXT("shellex\\"));
        SHStringFromGUID(riid, szSubKey + 8, CLSID_SIZE);

        TCHAR szCLSID[CLSID_SIZE];

        DWORD cbSize = sizeof(szCLSID);
        DWORD dwType;

        // should we test for a value as well as a key ?
        lRes = SHGetValue(hKey, szSubKey, TEXT(""), &dwType, szCLSID, &cbSize);
        RegCloseKey(hKey);

        if (lRes != ERROR_SUCCESS || dwType != REG_SZ)
        {
            return E_NOINTERFACE;
        }

        if (!GUIDFromString(szCLSID, &clsid))
        {
            return E_NOINTERFACE;
        }
        pCLSID = &clsid;
    }

    ASSERT(pCLSID);

    IPersistFile *pFile;
    HRESULT hr = CoCreateInstance(*pCLSID, NULL, CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARG(IPersistFile, &pFile));
    if (FAILED(hr))
    {
        return E_NOINTERFACE;
    }

    *ppvObj = NULL;

    hr = pFile->Load(pszBuffer, TRUE);
    if (SUCCEEDED(hr))
    {
        hr = pFile->QueryInterface(riid, ppvObj);
    }

    ATOMICRELEASE(pFile);

    return hr;
}

// routine used to make us think it really came from the right place....
HRESULT FakeGetUIObjectOf(IShellFolder *psf, LPCITEMIDLIST pidl, UINT * prgfFlags, REFIID riid, void **ppvObj)
{
    HRESULT hr = E_NOINTERFACE;

    if (WhichPlatform() == PLATFORM_INTEGRATED)
    {
        // we are on Nashville try the new mechanism first...
        hr = psf->GetUIObjectOf(NULL, 1, & pidl, riid, NULL, ppvObj);
        if (SUCCEEDED(hr))
        {
            return hr;
        }
    }

    // failure cases...
    if (riid == IID_IExtractImage || riid == IID_IExtractLogo || riid == IID_IQueryInfo)
    {
        // make sure this hacked up code is only executed for browser only release....
        // otherwise people will not register their stuff right and what a mess that will be.....
        if (WhichPlatform() == PLATFORM_INTEGRATED)
        {
            return hr;
        }

        // try the IconExtractor first ....
        IExtractIconA *pIcon;
        hr = psf->GetUIObjectOf(NULL, 1, &pidl, IID_X_PPV_ARG(IExtractIconA, NULL, &pIcon));
        if (SUCCEEDED(hr))
        {
            if (riid != IID_IQueryInfo)
            {
                hr = pIcon->QueryInterface(IID_IExtractLogo, ppvObj);
                ATOMICRELEASE(pIcon);

                if (SUCCEEDED(hr))
                    return NOERROR;
            }
            else
            {
                hr = pIcon->QueryInterface(IID_IQueryInfo, ppvObj);
                ATOMICRELEASE(pIcon);

                //if someone is asking for an IQueryInfo, don't try giving them an IExtractImage
                return hr;
            }
        }

        // browser mode only hack so we can detect if we are asking for the normal logo or the wide one...
        LPCTSTR pszTag = TEXT("Logo");
        if (prgfFlags != NULL && *prgfFlags)
        {
            pszTag = TEXT("WideLogo");
        }

        TCHAR szBuffer[MAX_PATH];
        hr = LookForDesktopIniText(psf, pidl, pszTag, szBuffer, ARRAYSIZE(szBuffer));
        if (SUCCEEDED(hr))
        {
            // use IID_IExtractImage, this is the same interface as IExtractLogo, just IExtractLogo
            // allows us to restrict the things that show up in Logo View...
            USES_CONVERSION;

            hr = LoadHandler(NULL, T2W(szBuffer), IID_IExtractImage, ppvObj);
        }
    }

    return hr;
}

BOOL GetInfoTipEx(IShellFolder* psf, DWORD dwFlags, LPCITEMIDLIST pidl, LPTSTR pszText, int cchTextMax)
{
    BOOL fRet = FALSE;

    *pszText = 0;   // empty for failure

    if (pidl)
    {
        IQueryInfo *pqi;
        if (SUCCEEDED(psf->GetUIObjectOf(NULL, 1, &pidl, IID_X_PPV_ARG(IQueryInfo, NULL, &pqi))))
        {
            WCHAR *pwszTip;
            pqi->GetInfoTip(dwFlags, &pwszTip);
            if (pwszTip)
            {
                fRet = TRUE;
                SHUnicodeToTChar(pwszTip, pszText, cchTextMax);
                SHFree(pwszTip);
            }
            pqi->Release();
        }
        else if (SUCCEEDED(FakeGetUIObjectOf(psf, pidl, 0, IID_PPV_ARG(IQueryInfo, &pqi))))
        {
            WCHAR *pwszTip;
            pqi->GetInfoTip(0, &pwszTip);
            if (pwszTip)
            {
                fRet = TRUE;
                SHUnicodeToTChar(pwszTip, pszText, cchTextMax);
                SHFree(pwszTip);
            }
            pqi->Release();
        }
    }
    return fRet;
}

BOOL GetInfoTip(IShellFolder* psf, LPCITEMIDLIST pidl, LPTSTR pszText, int cchTextMax)
{
    return GetInfoTipEx(psf, 0, pidl, pszText, cchTextMax);
}


#define MAX_CLASS   80  // From ..\shell32\fstreex.c
BOOL IsBrowsableShellExt(LPCITEMIDLIST pidl)
{
    DWORD    cb;
    LPCTSTR pszExt;
    TCHAR   szFile[MAX_PATH];
    TCHAR   szProgID[MAX_CLASS];
    TCHAR   szCLSID[GUIDSTR_MAX], szCATID[GUIDSTR_MAX];
    TCHAR   szKey[GUIDSTR_MAX * 4];
    HKEY    hkeyProgID = NULL;
    BOOL    fRet = FALSE;

    for (;;)
    {
        // Make sure we have a file extension
        if  (
            !SHGetPathFromIDList(pidl, szFile)
            ||
            ((pszExt = PathFindExtension(szFile)) == NULL)
            ||
            (pszExt[0] != TEXT('.'))
           )
        {
            break;
        }

        // Get the ProgID.
        cb = sizeof(szProgID);
        if  (
            (SHGetValue(HKEY_CLASSES_ROOT, pszExt, NULL, NULL, szProgID, &cb) != ERROR_SUCCESS)
            ||
            (RegOpenKey(HKEY_CLASSES_ROOT, szProgID, &hkeyProgID) != ERROR_SUCCESS)
           )
        {
            break;
        }

        // From the ProgID, get the CLSID.
        cb = sizeof(szCLSID);
        if (SHGetValue(hkeyProgID, TEXT("CLSID"), NULL, NULL, szCLSID, &cb) != ERROR_SUCCESS)
            break;

        // Construct the registry key that detects if
        // a CLSID is a member of a CATID.
        SHStringFromGUID(CATID_BrowsableShellExt, szCATID, ARRAYSIZE(szCATID));
        wnsprintf(szKey, ARRAYSIZE(szKey), TEXT("CLSID\\%s\\Implemented Categories\\%s"),
                 szCLSID, szCATID);

        // See if it's there.
        cb = 0;
        if (SHGetValue(HKEY_CLASSES_ROOT, szKey, NULL, NULL, NULL, &cb) != ERROR_SUCCESS)
            break;

        fRet = TRUE;
        break;
    }

    if (hkeyProgID != NULL)
        RegCloseKey(hkeyProgID);

    return fRet;
}


void OpenFolderPidl(LPCITEMIDLIST pidl)
{
    SHELLEXECUTEINFO shei = { 0 };

    shei.cbSize     = sizeof(shei);
    shei.fMask      = SEE_MASK_INVOKEIDLIST;
    shei.nShow      = SW_SHOWNORMAL;
    shei.lpIDList   = (LPITEMIDLIST)pidl;
    ShellExecuteEx(&shei);
}

void OpenFolderPath(LPCTSTR pszPath)
{
    LPITEMIDLIST pidl = ILCreateFromPath(pszPath);
    if (pidl)
    {
        OpenFolderPidl(pidl);
        ILFree(pidl);
    }
}

// NOTE: this is only called from browseui, why is it in the lib directory?
STDAPI UpdateSubscriptions()
{
#ifndef DISABLE_SUBSCRIPTIONS

    HRESULT hr;

    if (!SHRestricted2W(REST_NoManualUpdates, NULL, 0))
    {
        ISyncMgrSynchronizeInvoke *pSyncMgrInvoke;
        hr = CoCreateInstance(CLSID_SyncMgr, NULL, CLSCTX_ALL,
                              IID_PPV_ARG(ISyncMgrSynchronizeInvoke, &pSyncMgrInvoke));

        if (SUCCEEDED(hr))
        {
            hr = pSyncMgrInvoke->UpdateAll();
            pSyncMgrInvoke->Release();
        }
    }
    else
    {
        SHRestrictedMessageBox(NULL);
        hr = S_FALSE;
    }
    
    return hr;

#else  /* !DISABLE_SUBSCRIPTIONS */

    return E_FAIL;

#endif /* !DISABLE_SUBSCRIPTIONS */
}


STDAPI_(int) _SHHandleUpdateImage(LPCITEMIDLIST pidlExtra)
{
    SHChangeUpdateImageIDList * pUs = (SHChangeUpdateImageIDList*) pidlExtra;

    if (!pUs)
    {
        return -1;
    }

    // if in the same process, or an old style notification
    if (pUs->dwProcessID == GetCurrentProcessId())
    {
        return (int) pUs->iCurIndex;
    }
    else
    {
        WCHAR szBuffer[MAX_PATH];
        int iIconIndex = *(int UNALIGNED *)((BYTE *)&pUs->iIconIndex);
        UINT uFlags = *(UINT UNALIGNED *)((BYTE *)&pUs->uFlags);

        ualstrcpyW(szBuffer, pUs->szName);

        // we are in a different process, look up the hash in our index to get the right one...

        return Shell_GetCachedImageIndex(szBuffer, iIconIndex, uFlags);
    }
}

// As perf, share IShellLink implementations between bands and ask
// the bandsite for an implementation. Don't rely on the bandsite
// because you never know who will host us in the future. (And bandsite
// can change to not have us hosted at save/load time. Ex: it doesn't
// set our site before loading us from the stream, which sounds buggy.)
//
HRESULT SavePidlAsLink(IUnknown* punkSite, IStream *pstm, LPCITEMIDLIST pidl)
{
    HRESULT hr = E_FAIL;
    IShellLinkA* psl;

    if (punkSite)
        hr = IUnknown_QueryService(punkSite, IID_IBandSite, IID_PPV_ARG(IShellLinkA, &psl));
    if (FAILED(hr))
        hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARG(IShellLinkA, &psl));
    if (SUCCEEDED(hr))
    {
        IPersistStream *pps;
        hr = psl->QueryInterface(IID_PPV_ARG(IPersistStream, &pps));
        if (EVAL(SUCCEEDED(hr)))
        {
            ASSERT(pidl);
            psl->SetIDList(pidl);

            hr = pps->Save(pstm, FALSE);

            // Win95 and NT4 shell32 have a bug in the CShellLink implementation
            // THEY DON'T NULL TERMINATE THEIR "EXTRA DATA SECTION". This causes
            // the object to trash the rest of the stream when it reads back in.
            // Fix this by writing the NULL out in the Browser Only case.
            if (SUCCEEDED(hr) && (PLATFORM_BROWSERONLY == WhichPlatform()))
            {
                DWORD dw = 0;
                pstm->Write(&dw, sizeof(dw), NULL);
            }

            pps->Release();
        }
        psl->Release();
    }
    return hr;
}

HRESULT LoadPidlAsLink(IUnknown* punkSite, IStream *pstm, LPITEMIDLIST *ppidl)
{
    IShellLinkA* psl;
    HRESULT hr = IUnknown_QueryService(punkSite, IID_IBandSite, IID_PPV_ARG(IShellLinkA, &psl));
    if (FAILED(hr))
        hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARG(IShellLinkA, &psl));
    if (SUCCEEDED(hr))
    {
        IPersistStream *pps;
        hr = psl->QueryInterface(IID_PPV_ARG(IPersistStream, &pps));
        if (EVAL(SUCCEEDED(hr)))
        {
            hr = pps->Load(pstm);
            if (EVAL(SUCCEEDED(hr)))
            {
                hr = psl->GetIDList(ppidl);

                // Don't make me resolve the link because it's soo slow because
                // it often loads 80k of networking dlls.
                if (!EVAL(SUCCEEDED(hr)))
                {
                    hr = psl->Resolve(NULL, SLR_NOUPDATE | SLR_NO_UI);
                    if (EVAL(SUCCEEDED(hr)))
                        hr = psl->GetIDList(ppidl);
                }

                hr = *ppidl ? S_OK : E_FAIL;
            }
            pps->Release();
        }
        psl->Release();
    }
    return hr;
}



// AdjustECPosition
//
// purpose: because FE NT always uses WCHAR position for ComboBoxEx32
//          even though we're ANSI module for EM_GETSEL/EM_SETSEL,
//          we need to adjust between WCHAR and TCHAR position.
// iType:   ADJUST_TO_WCHAR_POS or ADJUST_TO_TCHAR_POS
//
int AdjustECPosition(char *psz, int iPos, int iType)
{
    char *pstr = psz;
    int iNewPos = iPos;

    if (psz && g_fRunOnFE && g_fRunningOnNT)
    {
        if (ADJUST_TO_WCHAR_POS == iType)
        {
            iNewPos = 0;
            while (*pstr && (pstr - psz != iPos))
            {
                pstr = CharNextA(pstr);
                iNewPos++;
            }
        }
        else if (ADJUST_TO_TCHAR_POS == iType)
        {
            while (*pstr && iPos--)
                pstr = CharNextA(pstr);
            iNewPos = (int)(pstr - psz);
        }
    }
    return iNewPos;
}

int CALLBACK _CompareIDs(LPARAM p1, LPARAM p2, LPARAM psf)
{
    HRESULT hr = ((IShellFolder*)psf)->CompareIDs(0, (LPITEMIDLIST)p1, (LPITEMIDLIST)p2);

    //ASSERT(SUCCEEDED(hr))
    return (short)HRESULT_CODE(hr);
}
HDPA GetSortedIDList(LPITEMIDLIST pidl)
{
    HDPA hdpa = DPA_Create(4);
    if (hdpa)
    {
        IShellFolder* psf;
        if (SUCCEEDED(IEBindToObject(pidl, &psf)))
        {
            LPENUMIDLIST penum;
            SHELLSTATE ss = {0};

            SHGetSetSettings(&ss, SSF_SHOWALLOBJECTS, FALSE);

            if (S_OK == IShellFolder_EnumObjects(psf, NULL,
                ss.fShowAllObjects ? SHCONTF_FOLDERS | SHCONTF_INCLUDEHIDDEN : SHCONTF_FOLDERS,
                &penum))
            {
                LPITEMIDLIST pidl;
                ULONG celt;
                while (penum->Next(1, &pidl, &celt) == S_OK && celt == 1)
                {
                    if (DPA_AppendPtr(hdpa, pidl) == -1)
                    {
                        SHFree(pidl);
                    }
                }
                penum->Release();
            }
            DPA_Sort(hdpa, (PFNDPACOMPARE)_CompareIDs, (LPARAM)psf);
            psf->Release();
        }
    }

    return hdpa;
}

int DPA_SHFreeCallback(void * p, void * d)
{
    SHFree((LPITEMIDLIST)p);
    return 1;
}

void FreeSortedIDList(HDPA hdpa)
{
    DPA_DestroyCallback(hdpa, (PFNDPAENUMCALLBACK)DPA_SHFreeCallback, 0);
    hdpa = NULL;
}

/****************************************************\
    FUNCTION: StrCmpIWithRoot

    PARAMETERS:
        szDispNameIn - Str to see if it is the same as the
                Display Name of the Root ISF.
        fTotalStrCmp - If TRUE, pszDispNameIn has to completely equal the
                Root's Display Name to succeed.  If FALSE, only the first part
                of pszDispNameIn needs to compare to the Root's Display Name
                for this function to return successful.
        ppszCachedRoot (In/Out Optional) - If this function will be called more than
                once, this function will cache the string and make it run
                quicker. The first time this function is called, (*ppszCachedRoot)
                needs to be NULL.  This function will allocate and the caller
                needs to call LocalFree() when it's no longer needed.

    DESCRIPTION:
        This function will get the Display Name of the Root ISF (Desktop) and
    see if the first cchDispNameComp chars of szDispNameIn
    match that display name.  S_OK will be returned if TRUE, and
    S_FALSE if not.
\****************************************************/
HRESULT StrCmpIWithRoot(LPCTSTR pszDispNameIn, BOOL fTotalStrCmp, LPTSTR * ppszCachedRoot)
{
    HRESULT hr;
    TCHAR szDispNameTemp[MAX_PATH];
    LPTSTR pszDispName = szDispNameTemp;

    ASSERT(IS_VALID_STRING_PTR(pszDispNameIn, -1));
    ASSERT(NULL == ppszCachedRoot || IS_VALID_WRITE_PTR(ppszCachedRoot, LPTSTR));

    // Did the caller supply the display name of the namespace root?
    if ((!ppszCachedRoot) ||
        (ppszCachedRoot && !*ppszCachedRoot))
    {
        MLLoadString(IDS_DESKTOP, szDispNameTemp, SIZECHARS(szDispNameTemp));

        // Cache this guy?
        if (ppszCachedRoot)
        {
            // Yes
            *ppszCachedRoot = StrDup(szDispNameTemp);
            if (!*ppszCachedRoot)
                return E_OUTOFMEMORY;
        }
    }

    if (ppszCachedRoot && *ppszCachedRoot)
        pszDispName = *ppszCachedRoot;

    // Do we want to compare the entire string or just the first part of it?
    if (fTotalStrCmp)
        hr = (0 == lstrcmpi(pszDispName, pszDispNameIn)) ? S_OK : S_FALSE;   // Entire String
    else if (ppszCachedRoot)
    {
        // Compare the first part of the string
        DWORD cchDispNameComp = lstrlen(*ppszCachedRoot);
        hr = (0 == StrCmpNI(pszDispName, pszDispNameIn, cchDispNameComp)) ? S_OK : S_FALSE;
    }
    else
    {
        hr = S_FALSE;
    }

    return hr;
}

/****************************************************\
    FUNCTION: GetMRUEntry

    PARAMETERS:
        hKey - Pointer to Registry Key to retrieve MRU entries from.
        dwMRUIndex - 0 based MRU Index to retrieve.
        pszMRUEntry - Location to store MRU Entry string.
        cchMRUEntry - Size of Buffer in characters.

    DESCRIPTION:
        This function will retrieve the MRU Entry specified
    by dwMRUIndex.
\****************************************************/
HRESULT GetMRUEntry(HKEY hKey, DWORD dwMRUIndex, LPTSTR pszMRUEntry, DWORD cchMRUEntry, LPITEMIDLIST * ppidl)
{
    HRESULT hr = S_OK;
    TCHAR szValueName[15];   // big enough for "url99999"

    ASSERT(hKey);
    ASSERT(pszMRUEntry);
    ASSERT(cchMRUEntry);

    // make a value name a la "url1" (1-based for historical reasons)
    wnsprintf(szValueName, ARRAYSIZE(szValueName), SZ_REGVAL_MRUENTRY, dwMRUIndex+1);
    cchMRUEntry *= sizeof(TCHAR);

    if (ERROR_SUCCESS != SHQueryValueEx(hKey, szValueName, NULL, NULL, (LPBYTE) pszMRUEntry, &cchMRUEntry))
    {
        pszMRUEntry[0] = TEXT('\0');
        hr = E_FAIL;
    }

    return hr;
}

/*----------------------------------------------------------
Purpose: Gets a registry value that is User Specifc.
         This will open HKEY_CURRENT_USER if it exists,
         otherwise it will open HKEY_LOCAL_MACHINE.

Returns: DWORD containing success or error code.
Cond:    --
*/
LONG OpenRegUSKey(LPCTSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
    DWORD dwRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, ulOptions, samDesired, phkResult);

    if (ERROR_SUCCESS != dwRet)
        dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, ulOptions, samDesired, phkResult);

    return dwRet;
}



typedef struct tagSTREAMHEADER
{
    DWORD dwHeaderSize;
    DWORD dwDataSize;
    DWORD dwSignature;
    DWORD dwVersion;
} STREAMHEADER;


/****************************************************\
    FUNCTION: LoadStreamHeader

    PARAMETERS:
        dwSignature - The signature that the caller supports
        dwStartVersion - The lowest version the caller chooses to support.
        dwEndVersion - The highest version the caller chooses to support.
        pdwSize (OUT) - The size that the caller should read.
        pdwVersionOut (OUT) - The version found.
        return - if S_OK, then the caller needs to read pdwSize bytes
                               of data.
                 if S_FALSE, then the caller should use default settings
                                  and return S_OK.

    DESCRIPTION:
        This function see if the caller owns this
    data in the stream.  If the caller does own the
    stream segment, the size and version will be returned.
    If the caller doesn't own the stream segment then S_FALSE is
    returned to indicate that the caller should use default
    data.  If the caller doesn't claim to support the version
    found (because of the dwStartVersion-dwEndVersion range),
    then S_FALSE is returned to indicate to use default values
    and this function skips over that segment in the stream
    so the next segment can be parsed.
\****************************************************/
HRESULT LoadStreamHeader(IStream *pstm,
                                    DWORD dwSignature,      // What version?
                                    DWORD dwStartVersion,   // What is the earlies version supported?
                                    DWORD dwEndVersion,     // What is the oldest version supported?
                                    DWORD * pdwSize,        // What is the size to read?
                                    DWORD * pdwVersionOut)  // What version was found in the stream?
{
    HRESULT hr;
    STREAMHEADER shHeader;
    BOOL fNotOurs = FALSE;
    BOOL fSkipData = FALSE;

    hr = pstm->Read(&shHeader, sizeof(shHeader), NULL);

    ASSERT(pdwSize && pdwVersionOut);
    *pdwSize = 0;
    *pdwVersionOut = 0;

    if (SUCCEEDED(hr))
    {
        if (shHeader.dwHeaderSize != sizeof(shHeader))
            fNotOurs = TRUE;
        else if (shHeader.dwSignature != dwSignature)
            fNotOurs = TRUE;
        else if (shHeader.dwVersion < dwStartVersion)
            fSkipData = TRUE;
        else if (shHeader.dwVersion > dwEndVersion)
            fSkipData = TRUE;

        if (fNotOurs)
        {
            // It's not, so reset it so the next guy will be able to read correctly.
            LARGE_INTEGER li;

            li.LowPart = (DWORD)-(LONG)sizeof(shHeader);
            li.HighPart = 0;

            hr = pstm->Seek(li, STREAM_SEEK_CUR, NULL);
            hr = S_FALSE; // Means caller should use default data.
        }

        // Do we want to skip the Data for this part of the stream?
        if (fSkipData)
        {
            ASSERT(STREAMSIZE_UNKNOWN != shHeader.dwDataSize);  // SERIOUS, we cannot skip over data because we don't know size.

            if (STREAMSIZE_UNKNOWN != shHeader.dwDataSize)
            {
                // Yes.  The caller cannot read in this data because the caller doesn't support
                // this version of the data.  Therefore, we skip past the data and return S_FALSE
                // to indicate to the caller that default settings should be used.
                LARGE_INTEGER li;

                li.LowPart = shHeader.dwDataSize;
                li.HighPart = 0;

                hr = pstm->Seek(li, STREAM_SEEK_CUR, NULL);
                hr = S_FALSE; // Means caller should use default data.
            }
        }

        if (!fNotOurs && !fSkipData)
        {
            *pdwSize = shHeader.dwDataSize;
            *pdwVersionOut = shHeader.dwVersion;
        }
    }

    return hr;
}


/****************************************************\
    FUNCTION: SaveStreamHeader

    DESCRIPTION:
        This function will save a StreamHeader to
    the stream that will allow the caller to verify
    if he/she owns the data the next time it's read in.
    It will also support the ability to ignore old
    or future versions of data.
\****************************************************/
HRESULT SaveStreamHeader(IStream *pstm, DWORD dwSignature, DWORD dwVersion, DWORD dwSize)
{
    HRESULT hr;
    STREAMHEADER shHeader;

    shHeader.dwHeaderSize = sizeof(STREAMHEADER);
    shHeader.dwDataSize = dwSize;
    shHeader.dwSignature = dwSignature;
    shHeader.dwVersion = dwVersion;

    hr = pstm->Write(&shHeader, sizeof(shHeader), NULL);
    return hr;
}


//----------------------------------------------------------------------
//
// CMenuList
//
//----------------------------------------------------------------------


typedef struct
{
    HMENU   hmenu;
    BITBOOL bObject:1;              // TRUE: menu belongs to object
} MLITEM;       // CMenuList item


CMenuList::CMenuList(void)
{
    ASSERT(NULL == _hdsa);
}


CMenuList::~CMenuList(void)
{
    if (_hdsa)
    {
        DSA_Destroy(_hdsa);
        _hdsa = NULL;
    }
}


/*----------------------------------------------------------
Purpose: Set the menu list (comparable to HOLEMENU) so we can
         dispatch commands to the frame or the object correctly.
         We do this since menu bands bypass OLE's FrameFilterWndProc.

         We build the menu list by comparing the given hmenuShared
         with hmenuFrame.  Anything in hmenuShared that is not
         in hmenuFrame belongs to the object.

*/
void CMenuList::Set(HMENU hmenuShared, HMENU hmenuFrame)
{
    ASSERT(NULL == hmenuShared || IS_VALID_HANDLE(hmenuShared, MENU));
    ASSERT(NULL == hmenuFrame || IS_VALID_HANDLE(hmenuFrame, MENU));

    if (_hdsa)
    {
        ASSERT(IS_VALID_HANDLE(_hdsa, DSA));

        DSA_DeleteAllItems(_hdsa);
    }
    else
        _hdsa = DSA_Create(sizeof(MLITEM), 10);

    if (_hdsa && hmenuShared && hmenuFrame)
    {
        int i;
        int iFrame = 0;
        int cmenu = GetMenuItemCount(hmenuShared);
        int cmenuFrame = GetMenuItemCount(hmenuFrame);
        BOOL bMatched;
        int iSaveFrame;
        int iHaveFrame = -1;

        TCHAR sz[64];
        TCHAR szFrame[64];
        MENUITEMINFO miiFrame;
        MENUITEMINFO mii;
        MLITEM mlitem;

        miiFrame.cbSize = sizeof(miiFrame);
        miiFrame.hSubMenu = NULL;
        mii.cbSize = sizeof(mii);

        for (i = 0; i < cmenu; i++)
        {

            mii.cch = SIZECHARS(sz);
            mii.fMask  = MIIM_SUBMENU | MIIM_TYPE;
            mii.dwTypeData = sz;
            EVAL(GetMenuItemInfoWrap(hmenuShared, i, TRUE, &mii));

            ASSERT(IS_VALID_HANDLE(mii.hSubMenu, MENU));

            mlitem.hmenu = mii.hSubMenu;

            iSaveFrame = iFrame;
            bMatched = FALSE;

            //  DocObject might have dropped some of our menus, like edit and view
            //  Need to be able to skip over dropped frame menus
            while (1)
            {
                if (iHaveFrame != iFrame)
                {
                    iHaveFrame = iFrame;
                    if (iFrame < cmenuFrame)
                    {
                        miiFrame.cch = SIZECHARS(szFrame);
                        miiFrame.fMask  = MIIM_SUBMENU | MIIM_TYPE;
                        miiFrame.dwTypeData = szFrame;
                        EVAL(GetMenuItemInfoWrap(hmenuFrame, iFrame, TRUE, &miiFrame));
                    }
                    else
                    {
                        // Make it so it won't compare
                        miiFrame.hSubMenu = NULL;
                        *szFrame = 0;
                    }

                }
                ASSERT(iFrame >= cmenuFrame || IS_VALID_HANDLE(miiFrame.hSubMenu, MENU));

                // The browser may have a menu that was not merged into
                // the shared menu because the object put one in with
                // the same name.  Have we hit this case? Check by comparing
                // sz and szFrame

                if (mii.hSubMenu == miiFrame.hSubMenu || 0 == StrCmp(sz, szFrame))
                {
                    bMatched = TRUE;
                    break;
                }
                else
                {
                    if (iFrame >= cmenuFrame)
                    {
                        break;
                    }
                    iFrame++;
                }
            }

            // Is this one of our menus?
            mlitem.bObject = (mii.hSubMenu == miiFrame.hSubMenu) ? FALSE:TRUE;
            if (bMatched)
            {
                iFrame++;
            }
            else
            {
                iFrame = iSaveFrame;
            }
            DSA_SetItem(_hdsa, i, &mlitem);
        }
    }
}


/*----------------------------------------------------------
Purpose: Adds the given hmenu to the list.

*/
void CMenuList::AddMenu(HMENU hmenu)
{
    ASSERT(NULL == hmenu || IS_VALID_HANDLE(hmenu, MENU));

    if (_hdsa && hmenu)
    {
        MLITEM mlitem;

        mlitem.hmenu = hmenu;
        mlitem.bObject = TRUE;

        DSA_AppendItem(_hdsa, &mlitem);
    }
}


/*----------------------------------------------------------
Purpose: Removes the given hmenu from the list.

*/
void CMenuList::RemoveMenu(HMENU hmenu)
{
    ASSERT(NULL == hmenu || IS_VALID_HANDLE(hmenu, MENU));

    if (_hdsa && hmenu)
    {
        int i = DSA_GetItemCount(_hdsa) - 1;

        for (; i >= 0; i--)
        {
            MLITEM * pmlitem = (MLITEM *)DSA_GetItemPtr(_hdsa, i);
            ASSERT(pmlitem);

            if (hmenu == pmlitem->hmenu)
            {
                DSA_DeleteItem(_hdsa, i);
                break;
            }
        }
    }
}


/*----------------------------------------------------------
Purpose: Returns TRUE if the given hmenu belongs to the object.

*/
BOOL CMenuList::IsObjectMenu(HMENU hmenu)
{
    BOOL bRet = FALSE;

    ASSERT(NULL == hmenu || IS_VALID_HANDLE(hmenu, MENU));

    if (_hdsa && hmenu)
    {
        int i;

        for (i = 0; i < DSA_GetItemCount(_hdsa); i++)
        {
            MLITEM * pmlitem = (MLITEM *)DSA_GetItemPtr(_hdsa, i);
            ASSERT(pmlitem);

            if (hmenu == pmlitem->hmenu)
            {
                bRet = pmlitem->bObject;
                break;
            }
        }
    }
    return bRet;
}


#ifdef DEBUG

void CMenuList::Dump(LPCTSTR pszMsg)
{
    if (IsFlagSet(g_dwDumpFlags, DF_DEBUGMENU))
    {
        TraceMsg(TF_ALWAYS, "CMenuList: Dumping menus for %#08x %s", this, pszMsg);

        if (_hdsa)
        {
            int i;

            for (i = 0; i < DSA_GetItemCount(_hdsa); i++)
            {
                MLITEM * pmlitem = (MLITEM *)DSA_GetItemPtr(_hdsa, i);
                ASSERT(pmlitem);

                TraceMsg(TF_ALWAYS, "   [%d] = %x", i, pmlitem->hmenu);
            }
        }
    }
}

#endif

#define REGVAL_FIRST_HOME_PAGE          TEXT("First Home Page")
#define REGVAL_UPDATE_CHECK_PAGE        TEXT("Update_Check_Page")
#define REGVAL_UPDATE_CHECK_INTERVAL    TEXT("Update_Check_Interval")
#define REGVAL_LASTCHECKEDHI            TEXT("LastCheckedHi")
#define REGSTR_PATH_INFODEL_REST        TEXT("Software\\Policies\\Microsoft\\Internet Explorer\\Infodelivery\\Restrictions")
#define REGVAL_IEUPDATECHECK_REST       TEXT("NoUpdateCheck")
#define DEFAULT_IEUPDATECHECK_PAGE      TEXT("http://www.microsoft.com/isapi/redir.dll?Prd=ie&Pver=5.0&Ar=ie5update&O1=b1")

BOOL
IsUpdateCheckRestricted()
{

    HKEY hkeyRest = 0;
    BOOL bUpdateCheckRest = FALSE;
    DWORD dwValue = 0;
    DWORD dwLen = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_INFODEL_REST, 0, KEY_READ, &hkeyRest) == ERROR_SUCCESS)
    {
        if (RegQueryValueEx(hkeyRest, REGVAL_IEUPDATECHECK_REST, NULL, NULL,
                      (LPBYTE)&dwValue, &dwLen) == ERROR_SUCCESS && dwValue)
            bUpdateCheckRest = TRUE;

        RegCloseKey(hkeyRest);
    }

    if (!bUpdateCheckRest)
    {
        // Check to see if the user has turned it off under advanced options
        dwValue = 0;
        dwLen = sizeof(DWORD);
        if (SHRegGetUSValue(REGSTR_PATH_MAIN, REGVAL_IEUPDATECHECK_REST, NULL, (LPBYTE)&dwValue, &dwLen, 0,NULL,0) == ERROR_SUCCESS && dwValue)
                bUpdateCheckRest = TRUE;
    }

    return bUpdateCheckRest;
}


HRESULT
CheckIEMinimalUpdate()
{
    HRESULT hr = S_OK;
    HKEY hkeyIE = 0;
    TCHAR szUpdateUrl[MAX_URL_STRING];
    DWORD dwSize;
    DWORD dwType;
    FILETIME ftlast, ftnow;
    DWORD dwMagicDays = 0;
    DWORD dwMagicPerDay = 201;

    if (IsUpdateCheckRestricted())
    {
        hr = HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
        goto Exit;
    }

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_MAIN, 0, KEY_READ, &hkeyIE) != ERROR_SUCCESS)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

    dwType = REG_DWORD;
    dwSize = sizeof(DWORD);

    if ((RegQueryValueEx(hkeyIE, REGVAL_UPDATE_CHECK_INTERVAL, NULL, &dwType,
            (unsigned char *)&dwMagicDays, &dwSize) != ERROR_SUCCESS) || dwMagicDays == 0)
    {
        dwMagicDays = 30;   // hardcode default to check every 30 days.
    }

    StrCpy(szUpdateUrl, DEFAULT_IEUPDATECHECK_PAGE);
    dwType = REG_SZ;
    dwSize = MAX_URL_STRING;
    if (RegQueryValueEx(hkeyIE, REGVAL_UPDATE_CHECK_PAGE, NULL, &dwType,
        (LPBYTE)szUpdateUrl, &dwSize) != ERROR_SUCCESS)
    {
        // go to default page on micorosoft.com
        //hr = HRESULT_FROM_WIN32(GetLastError());
        //goto Exit;
    }

    RegCloseKey(hkeyIE);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, REGSTR_PATH_MAIN, 0, KEY_READ|KEY_SET_VALUE, &hkeyIE) == ERROR_SUCCESS)
    {
        dwType = REG_SZ;
        dwSize = MAX_URL_STRING;
        if (RegQueryValueEx(hkeyIE, REGVAL_FIRST_HOME_PAGE, NULL, &dwType,
            NULL, &dwSize) == ERROR_SUCCESS)
        {
            // if already exists then skip this write
            hr = S_FALSE;
            goto Exit;
        }

        GetSystemTimeAsFileTime(&ftnow);
        ftnow.dwLowDateTime = 0;

        ZeroMemory(&ftlast, sizeof(ftlast));

        dwType = REG_DWORD;
        dwSize = sizeof(DWORD);
        if (RegQueryValueEx(hkeyIE, REGVAL_LASTCHECKEDHI, NULL, &dwType,
            (unsigned char *)&ftlast.dwHighDateTime, &dwSize)==ERROR_SUCCESS)
        {
            ftlast.dwHighDateTime += (dwMagicPerDay * dwMagicDays);
        }

        if (CompareFileTime(&ftlast, &ftnow) > 0)
        {
            hr = S_FALSE;
        }
        else
        {

            RegSetValueEx(hkeyIE,REGVAL_FIRST_HOME_PAGE, NULL,
                REG_SZ,(LPBYTE)szUpdateUrl, (lstrlen(szUpdateUrl)+1)*sizeof(TCHAR));

            RegSetValueEx(hkeyIE, REGVAL_LASTCHECKEDHI, NULL, REG_DWORD,
               (unsigned char *)&ftnow.dwHighDateTime, sizeof(DWORD));
        }

        RegCloseKey(hkeyIE);
    }

Exit:

    return hr;
}


static BOOL s_fSUCheckComplete = FALSE;
// returns:
//      TRUE    The user clicked Update Now and we ShellExe'ed
//              the update URL.
//      FALSE   We did not launch a browser to the update page.
// NOTE: the "run-once-ness" of this is controlled by the ICW check
//       variable g_fICWCheckComplete.

BOOL CheckSoftwareUpdateUI(HWND hwndOwner, IShellBrowser *pisb)
{
    BOOL fLaunchUpdate = FALSE;

#ifndef UNIX
    HRESULT hr = S_OK;
    int nRes;
    SOFTDISTINFO sdi = { 0 };
    sdi.cbSize = sizeof(SOFTDISTINFO);

    if (s_fSUCheckComplete)
        return FALSE;
    else
        s_fSUCheckComplete = TRUE;

    // We're putting up a message box, so make the msg pump modal
    pisb->EnableModelessSB(FALSE);

    nRes = SoftwareUpdateMessageBox(hwndOwner, awchMSIE4GUID, 0, &sdi);

    pisb->EnableModelessSB(TRUE);

    if (nRes != IDABORT)
    {
        if (nRes == IDYES)
        {
            // Okay, we tried to do this a couple of different ways.
            // Originally, this was done with ShellExecEx. This failed
            // because the http hook wasn't 100% reliable on Win95.
            // The next stab was to:
            //LPITEMIDLIST pidl;
            // The user wants to navigate to the install page.
            //hr = pibs->IEParseDisplayName(CP_ACP, sdi.szHREF, &pidl);
            //if (SUCCEEDED(hr))
            //{
            //    OpenFolderPidl(pidl);
            //    ILFree(pidl);
            //}
            hr = NavToUrlUsingIEW(sdi.szHREF, TRUE);

        } // if user wants update

        if (sdi.szTitle != NULL)
            CoTaskMemFree(sdi.szTitle);
        if (sdi.szAbstract != NULL)
            CoTaskMemFree(sdi.szAbstract);
        if (sdi.szHREF != NULL)
            CoTaskMemFree(sdi.szHREF);

        fLaunchUpdate = nRes == IDYES && SUCCEEDED(hr);

    }

    if (!fLaunchUpdate)
    {
        // for minimal install of IE every N days or so we want to
        // hijack the home page to check if an update is available
        // for us.

        CheckIEMinimalUpdate();
    }
#endif

    return fLaunchUpdate;
}



BOOL g_fICWCheckComplete = FALSE;

// returns:
//      TRUE    Internet Connection Wizard (ICW) was run, and we should exit
//              the browser since we likely need to restart the system
//      FALSE   did not run the ICW, continue on as normal

BOOL CheckRunICW(LPCTSTR pszURL)
{
    if (g_fICWCheckComplete)
        return FALSE;

    DWORD dwICWCompleted = 0;
    BOOL fRet = FALSE;

    // Check if ICW has already been run

    DWORD dwSize = sizeof(dwICWCompleted);
    SHGetValue(HKEY_CURRENT_USER, TEXT(ICW_REGPATHSETTINGS), TEXT(ICW_REGKEYCOMPLETED), NULL, &dwICWCompleted, &dwSize);

    if (!dwICWCompleted)
    {
        HINSTANCE hInetCfgDll = LoadLibrary(TEXT("inetcfg.dll"));
        // set this to TRUE here so that if there's an error in loading the dll, or getting the proc address,
        // we don't keep trying to do that.
        g_fICWCheckComplete = TRUE;
        if (hInetCfgDll)
        {
            PFNCHECKCONNECTIONWIZARD fp = (PFNCHECKCONNECTIONWIZARD)GetProcAddress(hInetCfgDll, "CheckConnectionWizard");
            if (fp)
            {
                DWORD dwRet;
                DWORD dwFlags = ICW_LAUNCHFULL | ICW_LAUNCHMANUAL | ICW_FULL_SMARTSTART;

                if (pszURL)
                {
                    PFNSETSHELLNEXT fpSetShellNext = (PFNSETSHELLNEXT)GetProcAddress(hInetCfgDll, "SetShellNext");
                    if (fpSetShellNext)
                    {
                        CHAR szAnsiUrl[MAX_URL_STRING];

                        SHTCharToAnsi(pszURL, szAnsiUrl, ARRAYSIZE(szAnsiUrl));
                        dwFlags |= ICW_USE_SHELLNEXT;
                        fpSetShellNext(szAnsiUrl);
                    }
                }

                // if we get this far, set the fICWCheckComplete back to FALSE (had to be false since we didn't early out)
                // and let the ICW set the reg key.  this is so that if the user decides to cancel and come back later,
                // we respect that.
                g_fICWCheckComplete = FALSE;

                // Launch ICW full or manual path, whichever is available
                // NOTE: the ICW code makes sure only a single instance is up
                fp(dwFlags, &dwRet);

                // If it was launched successfully, we need to exit
                // since ICW may restart the machine if it needs to
                // install system files.
                if (dwRet & (ICW_LAUNCHEDFULL | ICW_LAUNCHEDMANUAL))
                {
                    fRet = TRUE;
                }
            }
            FreeLibrary(hInetCfgDll);
        }
    }
    else
    {
        g_fICWCheckComplete = TRUE;
    }

    return fRet;
}


int GetColorComponent(LPSTR *ppsz)
{
    int iColor = 0;
    if (*ppsz)
    {
        LPSTR pBuf = *ppsz;
        iColor = StrToIntA(pBuf);

        // find the next comma
        while(pBuf && *pBuf && *pBuf!=L',')
            pBuf++;

        // if valid and not NULL...
        if (pBuf && *pBuf)
            pBuf++;         // increment

        *ppsz = pBuf;
    }
    return iColor;
}

// Read the registry for a string (REG_SZ) of comma separated RGB values
COLORREF RegGetColorRefString(HKEY hkey, LPTSTR RegValue, COLORREF Value)
{
    CHAR SmallBuf[80];
    CHAR szRegKey[MAXIMUM_SUB_KEY_LENGTH];
    LPSTR pszBuf;
    DWORD cb;
    int iRed, iGreen, iBlue;

    SHTCharToAnsi(RegValue, szRegKey, ARRAYSIZE(szRegKey));
    cb = sizeof(SmallBuf);
    if (SHQueryValueExA(hkey, szRegKey, NULL, NULL, (LPBYTE)&SmallBuf, &cb)
        == ERROR_SUCCESS)
    {
        pszBuf = SmallBuf;

        iRed = GetColorComponent(&pszBuf);
        iGreen = GetColorComponent(&pszBuf);
        iBlue = GetColorComponent(&pszBuf);

        // make sure all values are valid
        iRed    %= 256;
        iGreen  %= 256;
        iBlue   %= 256;

        Value = RGB(iRed, iGreen, iBlue);
    }

    return Value;
}

LRESULT SetHyperlinkCursor(IShellFolder* pShellFolder, LPCITEMIDLIST pidl)
{
    HCURSOR hCursor;
    BOOL fCursorSet = FALSE;

    if (!pidl)
        return 0;

    if (SHIsGlobalOffline())
    {
        IQueryInfo *pqi;
        if (SUCCEEDED(pShellFolder->GetUIObjectOf(NULL, 1, &pidl, IID_X_PPV_ARG(IQueryInfo, NULL, &pqi))))
        {
            DWORD dwFlags = 0;
            if (SUCCEEDED(pqi->GetInfoFlags(&dwFlags)))
            {
                if (0 == (dwFlags & QIF_CACHED))
                {
                    // Load Offline cursor since not cached
                    hCursor = (HCURSOR)LoadCursor(HINST_THISDLL, MAKEINTRESOURCE(IDC_OFFLINE_HAND));
                    if (hCursor)
                    {
                        SetCursor(hCursor);
                        fCursorSet = TRUE;
                    }
                }
            }
            pqi->Release();
        }
    }

    if (!fCursorSet)
    {
        // For whatever reason, offline cursor was not set
        hCursor = LoadHandCursor(0);
        if (hCursor)
            SetCursor(hCursor);
    }

    return 1;
}

BOOL IsSubscribableA(LPCSTR pszUrl)
{
    //  REARCHITECT: this should be method on the subscription mgr interface - zekel
    DWORD dwScheme = GetUrlSchemeA(pszUrl);
    return (dwScheme == URL_SCHEME_HTTP) || (dwScheme == URL_SCHEME_HTTPS);
}

BOOL IsSubscribableW(LPCWSTR pwzUrl)
{
    //  REARCHITECT: this should be method on the subscription mgr interface - zekel
    DWORD dwScheme = GetUrlSchemeW(pwzUrl);
    return (dwScheme == URL_SCHEME_HTTP) || (dwScheme == URL_SCHEME_HTTPS);
}

HWND GetTrayWindow()
{
#ifndef UNIX
    static HWND s_hwndTray = NULL;

    if (!IsWindow(s_hwndTray))
    {
        s_hwndTray = FindWindow(TEXT(WNDCLASS_TRAYNOTIFY), NULL);
    }
    return s_hwndTray;
#else
    return NULL;
#endif
}

void FireEventSzA(LPCSTR szEvent)
{
    HANDLE hEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, szEvent);
    if (hEvent)
    {
        SetEvent(hEvent);
        CloseHandle(hEvent);
    }
}

void FireEventSzW(LPCWSTR pszEvent)
{
    USES_CONVERSION;
    FireEventSzA(W2A(pszEvent));
}

BOOL IsNamedWindow(HWND hwnd, LPCTSTR pszClass)
{
#ifndef UNIX
    TCHAR szClass[32];
#else // UNIX use this function for trident dialog window
    TCHAR szClass[64];
#endif

    GetClassName(hwnd, szClass, ARRAYSIZE(szClass));
    return lstrcmp(szClass, pszClass) == 0;
}

BOOL IsExplorerWindow(HWND hwnd)
{
    return IsNamedWindow(hwnd, c_szExploreClass);
}

BOOL IsFolderWindow(HWND hwnd)
{
    TCHAR szClass[32];

    GetClassName(hwnd, szClass, ARRAYSIZE(szClass));
    return (lstrcmp(szClass, c_szCabinetClass) == 0) || (lstrcmp(szClass, c_szIExploreClass) == 0);
}


// returns TRUE if the unknown is on a window that was opened as IE
// returns FALSE if the window was opened on the shell namespace, even if it's now showing a web page
// returns FALSE in other cases e.g. on the taskbar

STDAPI_(BOOL) WasOpenedAsBrowser(IUnknown *punkSite) 
{
    // this is a more reliable way of distinguishing windows opened for a URL.  Checking
    // the hwnd's classname does not work -- clicking on a hyperlink from Outlook 98 opens
    // a browser window with a shell window's classname.

    return (S_OK == IUnknown_QueryServiceExec(punkSite, SID_STopLevelBrowser, &CGID_Explorer, SBCMDID_STARTEDFORINTERNET, 0, NULL, NULL));
}


#define DXTRACK 1
void FrameTrack(HDC hdc, LPRECT prc, UINT uFlags)
{
    COLORREF clrSave, clr;
    RECT    rc;

    // upperleft
    switch (uFlags)
    {
    case TRACKHOT:
        clr = GetSysColor(COLOR_BTNHILIGHT);
        break;
    case TRACKNOCHILD:
    case TRACKEXPAND:
        clr = GetSysColor(COLOR_BTNSHADOW);
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    clrSave = SetBkColor(hdc, clr);
    rc = *prc;
    rc.bottom = rc.top + DXTRACK;
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    rc.bottom = prc->bottom;
    rc.right = rc.left + DXTRACK;
    rc.top = prc->top + DXTRACK;
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    // lowerright
    switch (uFlags)
    {
    case TRACKHOT:
        clr = GetSysColor(COLOR_BTNSHADOW);
        break;
    case TRACKNOCHILD:
    case TRACKEXPAND:
        clr = GetSysColor(COLOR_BTNHILIGHT);
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    SetBkColor(hdc, clr);
    if (uFlags & (TRACKHOT | TRACKNOCHILD))
    {
        rc.right = prc->right;
        rc.top = rc.bottom - DXTRACK;
        rc.left = prc->left;
        ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    }
    rc.right = prc->right;
    rc.left = prc->right - DXTRACK;
    rc.top = prc->top;
    rc.bottom = prc->bottom;
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    SetBkColor(hdc, clrSave);
    return;
}

#ifdef DEBUG // {
//***   SearchDW -- scan for DWORD in buffer
// ENTRY/EXIT
//  pdwBuf  buffer
//  cbBuf   size of buffer in *bytes* (*not* DWORDs)
//  dwVal   DWORD we're looking for
//  dOff    (return) byte offset in buffer; o.w. -1 if not found
//
int SearchDWP(DWORD_PTR *pdwBuf, int cbBuf, DWORD_PTR dwVal)
{
    int dOff;

    for (dOff = 0; dOff < cbBuf; dOff += sizeof(DWORD_PTR), pdwBuf++)
    {
        if (*pdwBuf == dwVal)
            return dOff;
    }

    return -1;
}
#endif // }


int CAssociationList::FindEntry(DWORD dwKey)
{
    if (_hdsa)
    {
        for (int i = 0; i < DSA_GetItemCount(_hdsa); i++)
        {
            ASSOCDATA* pad;
            pad = (ASSOCDATA*)DSA_GetItemPtr(_hdsa, i);
            if (pad->dwKey == dwKey)
                return i;
        }
    }
    return -1;
}

HRESULT CAssociationList::Find(DWORD dwKey, void ** ppData)
{
    HRESULT hr = E_FAIL;

    ENTERCRITICAL;
    int i = FindEntry(dwKey);
    if (i != -1)
    {
        ASSOCDATA* pad = (ASSOCDATA*)DSA_GetItemPtr(_hdsa, i);
        ASSERT(dwKey == pad->dwKey);
        *ppData = pad->lpData;
        hr = S_OK;
    }
    LEAVECRITICAL;

    return hr;
}

void CAssociationList::Delete(DWORD dwKey)
{
    ENTERCRITICAL;
    int i = FindEntry(dwKey);
    if (i != -1)
    {
        DSA_DeleteItem(_hdsa, i);
    }
    LEAVECRITICAL;
}


BOOL CAssociationList::Add(DWORD dwKey, void *lpData)
{
    ENTERCRITICAL;
    if (!_hdsa)
    {
        _hdsa = DSA_Create(sizeof(ASSOCDATA), 4);
    }
    LEAVECRITICAL;

    BOOL fRet = FALSE;
    if (_hdsa)
    {
        ASSOCDATA ad;
        ad.dwKey = dwKey;
        ad.lpData = lpData;

        ENTERCRITICAL;
        fRet = DSA_AppendItem(_hdsa, &ad) != -1;
        LEAVECRITICAL;
    }
    return fRet;
}

int g_cxSmIcon = 0;
int g_cySmIcon = 0;
HIMAGELIST g_himlSysSmall = NULL;

void _InitSmallImageList()
{
    if (!g_himlSysSmall)
    {
        Shell_GetImageLists(NULL, &g_himlSysSmall);
        ImageList_GetIconSize(g_himlSysSmall, &g_cxSmIcon, &g_cySmIcon);
    }
}


#define CXIMAGEGAP      6

STDAPI_(void) DrawMenuItem(DRAWITEMSTRUCT* lpdi, LPCTSTR lpszMenuText, UINT iIcon)
{
    _InitSmallImageList();

    if ((lpdi->itemAction & ODA_SELECT) || (lpdi->itemAction & ODA_DRAWENTIRE))
    {
        int x, y;
        SIZE sz;
        RECT rc;

        // Draw the image (if there is one).
        GetTextExtentPoint32(lpdi->hDC, lpszMenuText, lstrlen(lpszMenuText), &sz);

        if (lpdi->itemState & ODS_SELECTED)
        {
            SetBkColor(lpdi->hDC, GetSysColor(COLOR_HIGHLIGHT));
            SetTextColor(lpdi->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
            FillRect(lpdi->hDC,&lpdi->rcItem,GetSysColorBrush(COLOR_HIGHLIGHT));
        }
        else
        {
            SetTextColor(lpdi->hDC, GetSysColor(COLOR_MENUTEXT));
            FillRect(lpdi->hDC,&lpdi->rcItem,GetSysColorBrush(COLOR_MENU));
        }

        rc = lpdi->rcItem;
        rc.left += +2 * CXIMAGEGAP + g_cxSmIcon;

        DrawText(lpdi->hDC, lpszMenuText, lstrlen(lpszMenuText), &rc, DT_SINGLELINE | DT_VCENTER | DT_EXPANDTABS);
        if (iIcon != -1)
        {
            x = lpdi->rcItem.left + CXIMAGEGAP;
            y = (lpdi->rcItem.bottom + lpdi->rcItem.top - g_cySmIcon) / 2;
            ImageList_Draw(g_himlSysSmall, iIcon, lpdi->hDC, x, y, ILD_TRANSPARENT);
        }
        else
        {
            x = lpdi->rcItem.left + CXIMAGEGAP;
            y = (lpdi->rcItem.bottom + lpdi->rcItem.top - g_cySmIcon) / 2;
        }
    }
}

STDAPI_(LRESULT) MeasureMenuItem(MEASUREITEMSTRUCT *lpmi, LPCTSTR lpszMenuText)
{
    LRESULT lres = FALSE;

    if (0 == g_cxSmIcon)
    {
        _InitSmallImageList();
    }

    // Get the rough height of an item so we can work out when to break the
    // menu. User should really do this for us but that would be useful.
    HDC hdc = GetDC(NULL);
    if (hdc)
    {
        // REVIEW cache out the menu font?
        NONCLIENTMETRICSA ncm;
        ncm.cbSize = sizeof(ncm);
        if (SystemParametersInfoA(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, FALSE))
        {
            HFONT hfont = CreateFontIndirectA(&ncm.lfMenuFont);
            if (hfont)
            {
                SIZE sz;
                HFONT hfontOld = (HFONT)SelectObject(hdc, hfont);
                GetTextExtentPoint32(hdc, lpszMenuText, lstrlen(lpszMenuText), &sz);
                lpmi->itemHeight = max (g_cySmIcon+CXIMAGEGAP/2, ncm.iMenuHeight);
                lpmi->itemWidth = g_cxSmIcon + 2*CXIMAGEGAP + sz.cx;
                SelectObject(hdc, hfontOld);
                DeleteObject(hfont);
                lres = TRUE;
            }
        }
        ReleaseDC(NULL, hdc);
    }
    return lres;
}

//+-------------------------------------------------------------------------
// This function scans the document for the given HTML tag and returns the
// result in a collection.
//--------------------------------------------------------------------------
HRESULT GetDocumentTags
(
    IHTMLDocument2 *          pHTMLDocument,    // doc to search
    LPOLESTR                  pszTagName,       // tag name to search for
    IHTMLElementCollection ** ppTagsCollection  // returned collection
)
{
    HRESULT hr;

    *ppTagsCollection = NULL;

    //
    // First get all document elements
    //
    IHTMLElementCollection * pAllCollection;
    if (SUCCEEDED(hr = pHTMLDocument->get_all(&pAllCollection)))
    {
        //
        // Now get all the elements with tags == pszTagName
        //
        VARIANT v;
        v.vt = VT_BSTR;
        v.bstrVal = ::SysAllocString(pszTagName);
        if (v.bstrVal)
        {
            IDispatch * pDispTagsCollection;
            if (SUCCEEDED(hr = pAllCollection->tags(v, &pDispTagsCollection)))
            {
                hr = pDispTagsCollection->QueryInterface(IID_PPV_ARG(IHTMLElementCollection, ppTagsCollection));
                pDispTagsCollection->Release();
            }

            pAllCollection->Release();
            VariantClear(&v);
        }
    }

    return hr;
}

// This function uses the memory allocator from comctrl (which differs between NT and W95)
BOOL WINAPI Str_SetPtrPrivateW(WCHAR FAR * UNALIGNED * ppwzCurrent, LPCWSTR pwzNew)
{
    LPWSTR pwzNewCopy = NULL;

    if (pwzNew)
    {
        pwzNewCopy = StrDup(pwzNew);
        if (!pwzNewCopy)
            return FALSE;
    }

    LPWSTR pwzOld = (LPWSTR)InterlockedExchangePointer((void * *)ppwzCurrent, (void *)pwzNewCopy);
    if (pwzOld)
        LocalFree(pwzOld);

    return TRUE;
}

// This function is compatible with API's that use LocalAlloc for string memory
BOOL WINAPI SetStr(WCHAR FAR * UNALIGNED * ppwzCurrent, LPCWSTR pwzNew)
{
    int cchLength;
    LPWSTR pwzOld;
    LPWSTR pwzNewCopy = NULL;

    if (pwzNew)
    {
        cchLength = lstrlenW(pwzNew);

        // alloc a new buffer w/ room for the null terminator
        pwzNewCopy = (LPWSTR)LocalAlloc(LPTR, (cchLength + 1) * sizeof(WCHAR));

        if (!pwzNewCopy)
            return FALSE;

        StrCpyNW(pwzNewCopy, pwzNew, cchLength + 1);
    }

    pwzOld = (LPWSTR)InterlockedExchangePointer((void * *)ppwzCurrent, (void *)pwzNewCopy);

    if (pwzOld)
        LocalFree(pwzOld);

    return TRUE;
}

//---------------------------------------------------------------------------
// If the string contains &ch or begins with ch then return TRUE.
BOOL _MenuCharMatch(LPCTSTR lpsz, TCHAR ch, BOOL fIgnoreAmpersand)
{
    LPTSTR pchAS = StrChr(lpsz, TEXT('&')); // Find the first ampersand.
    if (pchAS && !fIgnoreAmpersand)
    {
        // Yep, is the next char the one we want.
        if (CharUpperChar(*CharNext(pchAS)) == CharUpperChar(ch))
        {
            // Yep.
            return TRUE;
        }
    }
    else if (CharUpperChar(*lpsz) == CharUpperChar(ch))
    {
        return TRUE;
    }

    return FALSE;
}

// Review chrisny:  this can be moved into an object easily to handle generic droptarget, dropcursor
// , autoscrool, etc. . .
void _DragEnter(HWND hwndTarget, const POINTL ptStart, IDataObject *pdtObject)
{
    RECT    rc;
    POINT   pt;

    GetWindowRect(hwndTarget, &rc);

    //
    // If hwndTarget is RTL mirrored, then measure the
    // the client point from the visual right edge
    // (near edge in RTL mirrored windows). [samera]
    //
    if (IS_WINDOW_RTL_MIRRORED(hwndTarget))
        pt.x = rc.right - ptStart.x;
    else
        pt.x = ptStart.x - rc.left;

    pt.y = ptStart.y - rc.top;
    DAD_DragEnterEx2(hwndTarget, pt, pdtObject);
    return;
}

void _DragMove(HWND hwndTarget, const POINTL ptStart)
{
    RECT rc;
    POINT pt;

    GetWindowRect(hwndTarget, &rc);

    //
    // If hwndTarget is RTL mirrored, then measure the
    // the client point from the visual right edge
    // (near edge in RTL mirrored windows). [samera]
    //
    if (IS_WINDOW_RTL_MIRRORED(hwndTarget))
        pt.x = rc.right - ptStart.x;
    else
        pt.x = ptStart.x - rc.left;

    pt.y = ptStart.y - rc.top;
    DAD_DragMove(pt);
    return;
}



HRESULT CheckDesktopIni(LPCTSTR pszPath, LPCTSTR pszKey, LPTSTR pszBuffer, DWORD cbSize)
{
    // NOTE:
    // NOTE: DO NOT COPY THIS CODE. We only do this here for channels because we expect
    // NOTE: the 99% case to be that it succeeds. If you need to find out if it is a
    // NOTE: system folder, then you need to hack the pidl to get the system bit
    // NOTE:
    DWORD dwAttrs = GetFileAttributes(pszPath);
    if (dwAttrs == (DWORD) -1 || !(dwAttrs & (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY)))
        return E_NOINTERFACE;

    TCHAR szDIPath[MAX_PATH];

    PathCombine(szDIPath, pszPath, TEXT("desktop.ini"));
    if (pszKey == NULL)
    {
        if (GetFileAttributes(szDIPath) == (DWORD) -1)
        {
            return E_FAIL;
        }
    }
    else
    {
        GetPrivateProfileString(TEXT(".ShellClassInfo"), pszKey, TEXT(""), pszBuffer, cbSize, szDIPath);
        if (*pszBuffer == 0)
            return E_NOINTERFACE;

        // if its not a URL, then
        if (!PathIsURL(pszBuffer))
        {
            PathCombine(pszBuffer, pszPath, pszBuffer);
        }
    }
    return NOERROR;
}

STDAPI LookForDesktopIniText(IShellFolder *psf, LPCITEMIDLIST pidl, LPCTSTR pszKey, LPTSTR pszBuffer, DWORD cbSize)
{
    TCHAR szPath[MAX_PATH];
    DWORD ulFlags = SFGAO_FOLDER | SFGAO_FILESYSTEM;
    HRESULT hr = GetPathForItem(psf, pidl, szPath, &ulFlags);
    if (SUCCEEDED(hr) && (ulFlags & SFGAO_FOLDER))
    {
        hr = CheckDesktopIni(szPath, pszKey, pszBuffer, cbSize);
    }
    return hr;
}

// this is for channel category folders

HRESULT FakeGetNavigateTarget(IShellFolder *psf, LPCITEMIDLIST pidl, LPITEMIDLIST *ppidl)
{
    HRESULT hres = E_FAIL;

    WIN32_FIND_DATAA wfd;

    // before looking for a desktop.ini (which hits the disk), cheaply
    // see if it's a system folder.
    //
    // SHGetDataFromIDListA returns E_INVALIDARG on IE4.0 shell32 and
    // IE4.01 shell32.  It is fixed in IE4.01qfe shell32 and IE4.01sp1
    // shell32.  It also appears to work on NT4 and W95 shell32.  If
    // SHGetDataFromIDListA returns E_INVALIDARG we drop through and
    // do the slow LookForDesktopIniText call.
    //

    HRESULT hresTemp = SHGetDataFromIDListA(psf, pidl, SHGDFIL_FINDDATA, &wfd, sizeof(wfd));

    // on win95 non integrated, only the A version is implemented
    if ((E_INVALIDARG == hresTemp) ||
        (SUCCEEDED(hresTemp) &&
        (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
        (wfd.dwFileAttributes & (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY))))
    {
        // final failure case, check to see if it is a system folder with a desktop.ini file. This is gross and slow,
        // but this is only called when the user has navigated so we can be a little slower. Please do NOT COPY this
        // code or DavidDS will get really really upset and perf will be bad.
        TCHAR szBuffer[MAX_URL_STRING];

        // this will check for SFGAO_FOLDER & SFGAO_FILESYSTEM first...
        hres = LookForDesktopIniText(psf, pidl, TEXT("URL"), szBuffer, ARRAYSIZE(szBuffer));
        if (SUCCEEDED(hres))
        {
            DWORD dwChar = ARRAYSIZE(szBuffer);
            // this call uses a temp, so we can reuse the buffer...
            hres = UrlCreateFromPath(szBuffer, szBuffer, &dwChar, 0);
            if (SUCCEEDED(hres))
                hres = IECreateFromPath(szBuffer, ppidl);
        }
    }
    else
    {
        hres = E_FAIL;
    }
    return hres;
}

// Can we browse or navigate to this pidl?  If not, need
BOOL ILIsBrowsable(LPCITEMIDLIST pidl, BOOL *pfIsFolder)
{
    if (!pidl)
        return FALSE;
    DWORD dwAttributes = SFGAO_FOLDER | SFGAO_BROWSABLE;
    HRESULT hr = IEGetAttributesOf(pidl, &dwAttributes);

    if (pfIsFolder && SUCCEEDED(hr))
        *pfIsFolder = dwAttributes & SFGAO_FOLDER;

    return SUCCEEDED(hr) && (dwAttributes & (SFGAO_FOLDER | SFGAO_BROWSABLE));
}



// gets a target pidl given a name space item. typically this is a .lnk or .url file
//
//  in:
//        psf         shell folder for item
//        pidl        item relative to psf, single level
//
//  in/out
//        pdwAttribs  [optional] attributes mask to filter on (returned).
//        must be initalized
//
//
//  returns
//        *ppidl      the target pidl
//        *pdwAttribs [optional] attributes of the source object

STDAPI SHGetNavigateTarget(IShellFolder *psf, LPCITEMIDLIST pidl,
                         LPITEMIDLIST *ppidl, DWORD *pdwAttribs)
{
    ASSERT(IS_VALID_CODE_PTR(psf, IShellFolder));
    ASSERT(IS_VALID_PIDL(pidl));
    ASSERT(NULL == pdwAttribs || IS_VALID_WRITE_PTR(pdwAttribs, DWORD));
    ASSERT(ILFindLastID(pidl) == pidl);   // must be single level PIDL

    *ppidl = NULL;      // assume failure

    DWORD dwAttribs = SFGAO_FILESYSTEM | SFGAO_FOLDER | SFGAO_LINK | SFGAO_BROWSABLE;

    if (pdwAttribs)
        dwAttribs |= *pdwAttribs;

    HRESULT hres = psf->GetAttributesOf(1, &pidl, &dwAttribs);
    if (SUCCEEDED(hres))
    {
        // first try the most efficient way
        IShellLinkA *psl;       // "A" so this works on Win95
        hres = psf->GetUIObjectOf(NULL, 1, &pidl, IID_X_PPV_ARG(IShellLinkA, NULL, &psl));
        if (SUCCEEDED(hres))
        {
            hres = psl->GetIDList(ppidl);
            psl->Release();
        }

        // this is for .lnk and .url files that don't register properly
        if (FAILED(hres) && (dwAttribs & (SFGAO_FILESYSTEM | SFGAO_LINK)) == (SFGAO_FILESYSTEM | SFGAO_LINK))
        {
            TCHAR szPath[MAX_PATH];

            hres = GetPathForItem(psf, pidl, szPath, NULL);
            if (SUCCEEDED(hres))
                hres = GetLinkTargetIDList(szPath, NULL, 0, ppidl);
        }

        // .doc or .html. return the pidl for this.
        // (fully qualify against the folder pidl)
        if (FAILED(hres) && (dwAttribs & SFGAO_BROWSABLE))
        {
            LPITEMIDLIST pidlFolder;
            hres = SHGetIDListFromUnk(psf, &pidlFolder);
            if (SUCCEEDED(hres))
            {
                *ppidl = ILCombine(pidlFolder, pidl); // navigate to this thing...
                hres = *ppidl ? S_OK : E_OUTOFMEMORY;
                ILFree(pidlFolder);
            }
        }

        // channel name space items on non integrated
        if (FAILED(hres) && WhichPlatform() != PLATFORM_INTEGRATED)
        {
            IExtractIconA *pxicon;   // Use IID_IExtractIconA so we work on W95.
            hres = psf->GetUIObjectOf(NULL, 1, &pidl, IID_X_PPV_ARG(IExtractIconA, NULL, &pxicon));
            if (SUCCEEDED(hres))
            {
                hres = pxicon->QueryInterface(IID_PPV_ARG(IShellLinkA, &psl));
                if (SUCCEEDED(hres))
                {
                    hres = psl->GetIDList(ppidl);
                    psl->Release();
                }
                pxicon->Release();
            }
        }

#ifdef ENABLE_CHANNEL_CATEGORY_FOLDERS
        // channel category folders
        if (FAILED(hres))
            hres = FakeGetNavigateTarget(psf, pidl, ppidl);
#endif //ENABLE_CHANNEL_CATEGORY_FOLDERS

        // Callers of SHGetNavigateTarget assume that the returned pidl
        // is navigatable (SFGAO_FOLDER or SFGAO_BROWSER), which isn't
        // the case for a link (it could be a link to an exe).
        //
        if (SUCCEEDED(hres) && !ILIsBrowsable(*ppidl, NULL))
        {
            ILFree(*ppidl);
            *ppidl = NULL;
            hres = E_FAIL;
        }

        if (SUCCEEDED(hres) && pdwAttribs)
            *pdwAttribs = dwAttribs;
    }
    return hres;
}

BOOL CreateShortcutAndDoDragDropIfPIDLIsNetUrl(IOleCommandTarget *pcmdt, LPITEMIDLIST pidl, HWND hwnd)
{
    IUniformResourceLocator *purl;
    IDataObject *pdtobj;

    HRESULT hr = CreateShortcutSetSiteAndGetDataObjectIfPIDLIsNetUrl(pidl, pcmdt, &purl, &pdtobj);

    if (SUCCEEDED(hr))
    {
        ASSERT(pdtobj);
        ASSERT(purl);

        // REARCHITECT: we should be binding to the parent and getting the attributes
        // to determine the allowed effects - like we do in DragDrop()
        DWORD dwEffect = (DROPEFFECT_COPY | DROPEFFECT_LINK);

        ::_SetPreferedDropEffect(pdtobj, DROPEFFECT_LINK);
        // Win95 Browser Only - the shell32 in this process doesn't know
        // ole is loaded, even though it is.
        SHLoadOLE(SHELLNOTIFY_OLELOADED);

        hr = SHDoDragDrop(hwnd, pdtobj, NULL, dwEffect, &dwEffect);
        // the returned value is not S_OK even tho' the drag drop succeeded
        // however it is a success return
        if (SUCCEEDED(hr))
        {
            // Since drag drop succeeded
            // Bring down the icon for this shortcut
            IUnknown_Exec(purl, &CGID_ShortCut, ISHCUTCMDID_DOWNLOADICON, 0, NULL, NULL);
        }
        pdtobj->Release();
        IUnknown_SetSite(purl, NULL);
        purl->Release();
    }

    return SUCCEEDED(hr);
}

BOOL DoDragDropWithInternetShortcut(IOleCommandTarget *pcmdt, LPITEMIDLIST pidl, HWND hwnd)
{
    BOOL fDragDropDone = CreateShortcutAndDoDragDropIfPIDLIsNetUrl(pcmdt, pidl, hwnd);
    if (FALSE == fDragDropDone)
    {
        // simply use PIDL and get none of the persistence effect
        fDragDropDone = SUCCEEDED(DragDrop(hwnd, NULL, pidl, DROPEFFECT_LINK, NULL));
    }
    return fDragDropDone;
}

STDAPI_(HWND) GetTopLevelAncestor(HWND hWnd)
{
    HWND hwndTemp;

    while ((hwndTemp=GetParent(hWnd)) != NULL)
    {
        hWnd = hwndTemp;
    }

    return(hWnd);
}

#if 0
BOOL IsIERepairOn()
{
    static DWORD     dwChecked = -1;

    if (dwChecked == -1)
    {
        DWORD   dwSize, dwType;

        // First check the OS setting. On NT5 and Win98-OSR, Repair is Off.
        // OS turned Off Repair ==> "DisableRepair" RegValue is set to 1.
        dwChecked = 1;       // The default Repair is ON
        dwSize = sizeof(dwChecked);
        if (SHRegGetUSValue(SZ_REGKEY_ACTIVE_SETUP, SZ_REGVALUE_DISABLE_REPAIR, &dwType, (void *) &dwChecked, &dwSize, TRUE, (void *)NULL, 0) == ERROR_SUCCESS)
        {
            // OS Reg setting of 0 ==> Repair is ON
            // OS Reg setting of 1 ==> Repair is OFF
            dwChecked = (dwChecked == 0) ? 1 : 0;
        }
        else
        {
            dwChecked = 1;   // if we fail to read Reg, go back to default.
        }

        // Check for Admin policy only if OS setting leaves Repair On.
        if (dwChecked == 1)
        {
            dwSize = sizeof(dwChecked);
            if (SHRegGetUSValue(SZ_REGKEY_IE_POLICIES, SZ_REGVALUE_IEREPAIR, &dwType, (void *) &dwChecked, &dwSize, TRUE, (void *)NULL, 0) != ERROR_SUCCESS)
            {
                dwChecked = 1;   // if we fail to read Reg, go back to default.
            }
        }
    }
    return (dwChecked == 1);
}

#endif

BOOL IsResetWebSettingsEnabled(void)
{
    static BOOL fUseCache = FALSE;  // have we already looked up the answer in the registry?
    static BOOL fEnabled;           // is the feature enabled or disabled?

    if (!fUseCache)
    {

        DWORD dwData;
        DWORD dwSize = sizeof(dwData);
        DWORD dwType;

        //
        // Next time, we'll use the cached value instead of
        // looking in the registry
        //
        fUseCache = TRUE;

        //
        // Look up the appropriate ieak value in the registry
        //
        if (ERROR_SUCCESS == SHRegGetUSValue(
                                SZ_REGKEY_INETCPL_POLICIES,
                                SZ_REGVALUE_RESETWEBSETTINGS,
                                &dwType,
                                (void *)&dwData,
                                &dwSize,
                                FALSE,
                                NULL,
                                0))
        {
            //
            // If the value was found in the registry, then
            // set fEnabled accordingly
            //
            fEnabled = !dwData;

        }
        else
        {
            //
            // If the value is missing from the registry, then
            // assume the feature is enabled
            //
            fEnabled = TRUE;

        }

    }

    return fEnabled;

}

STDAPI_(BOOL) InitOCHostClass(const SHDRC * pshdrc)
{
    // It would be nice to remove this, but since it was exported, we keep it here for compat
    RIPMSG(FALSE, "This export is dead, caller needs to call SHDOCVW!DllRegisterWindowClasses directly");
    return DllRegisterWindowClasses(pshdrc);
}

STDAPI SHNavigateToFavorite(IShellFolder* psf, LPCITEMIDLIST pidl, IUnknown* punkSite, DWORD dwFlags)
{
    HRESULT hres = S_FALSE;

    TCHAR szPath[MAX_PATH];

    // Can we navigate to this favorite?
    BOOL fNavigateDone = SUCCEEDED(GetPathForItem(psf, pidl, szPath, NULL)) &&
                         SUCCEEDED(NavFrameWithFile(szPath, punkSite));
    if (fNavigateDone)
        return S_OK;

    LPITEMIDLIST pidlGoto;

    ASSERT(!(dwFlags & (SBSP_NEWBROWSER | SBSP_SAMEBROWSER)));
    
    if (SUCCEEDED(SHGetNavigateTarget(psf, pidl, &pidlGoto, NULL)))
    {
        IShellBrowser* psb;
        if (SUCCEEDED(IUnknown_QueryService(punkSite, SID_STopLevelBrowser,
            IID_PPV_ARG(IShellBrowser, &psb))))
        {
            hres = psb->BrowseObject(pidlGoto, dwFlags | SBSP_SAMEBROWSER);
            psb->Release();
        }
        ILFree(pidlGoto);
    }
    return hres;
}
STDAPI SHGetTopBrowserWindow(IUnknown* punk, HWND* phwnd)
{
    IOleWindow* pOleWindow;
    HRESULT hr = IUnknown_QueryService(punk, SID_STopLevelBrowser, IID_PPV_ARG(IOleWindow, &pOleWindow));
    if (SUCCEEDED(hr))
    {
        hr = pOleWindow->GetWindow(phwnd);
        pOleWindow->Release();
    }
    return hr;
}


BOOL ILIsFolder(LPCITEMIDLIST pidl)
{
    BOOL fIsFolder = FALSE;
    DWORD dwAttributes = SFGAO_FOLDER;
    HRESULT hr = IEGetAttributesOf(pidl, &dwAttributes);

    if (SFGAO_FOLDER == dwAttributes)
        fIsFolder = TRUE;

    return fIsFolder;
}


STDAPI_(LPITEMIDLIST) IEGetInternetRootID(void)
{
    LPITEMIDLIST pidl;

    //
    //  HACKHACK - we want the pidl to the Internet SF
    //  so we make a dummy URL and parse it.  then
    //  we know its parent will be the Internet SF.
    //
    if (SUCCEEDED(IECreateFromPath(TEXT("dummy://url"), &pidl)))
    {
        ASSERT(!ILIsEmpty(_ILNext(pidl)));
        ASSERT(IsURLChild(pidl, FALSE));

        //  we only want the parent Internt SF
        _ILNext(pidl)->mkid.cb = 0;
        return pidl;
    }
    return NULL;
}

STDAPI_(void) UpdateButtonArray(TBBUTTON *ptbDst, const TBBUTTON *ptbSrc, int ctb, LONG_PTR lStrOffset)
{
    memcpy(ptbDst, ptbSrc, ctb*sizeof(TBBUTTON));
    if (lStrOffset == -1)
    {
        // handle failure case
        for (int i = 0; i < ctb; i++)
            ptbDst[i].iString = 0;
    }
    else
    {
        for (int i = 0; i < ctb; i++)
            ptbDst[i].iString += lStrOffset;
    }
}

//----------------------------------------------------------------------------
//  <Swipped from the NT5 version of Shell32>
//
STDAPI_(void) PathToAppPathKey(LPCTSTR pszPath, LPTSTR pszKey, int cchKey)
{
    // Use the szTemp variable of pseem to build key to the programs specific
    // key in the registry as well as other things...
    StrCpyN(pszKey, REGSTR_PATH_APPPATHS, cchKey);
    StrCatBuff(pszKey, TEXT("\\"), cchKey);
    StrCatBuff(pszKey, PathFindFileName(pszPath), cchKey);

    // Currently we will only look up .EXE if an extension is not
    // specified
    if (*PathFindExtension(pszKey) == 0)
    {
        StrCatBuff(pszKey, TEXT(".exe"), cchKey);
    }
}

//----------------------------------------------------------------------------
//  <Swipped from the NT5 version of Shell32>
//
// this function checks for the existance of a value called "useURL" under the
// App Paths key in the registry associated with the app that is passed in.

STDAPI_(BOOL) DoesAppWantUrl(LPCTSTR pszCmdLine)
{
    TCHAR szRegKeyName[MAX_PATH];
    HKEY hKeyAppPaths;
    BOOL bRet = FALSE;

// bug 61538 - The edit button never passes in args or quotes and the 
// code below was screwing up if there were spaces in the path.
//
    // need to copy the string since PathRemoveArgs whacks in a \0
//    TCHAR szTemp[MAX_PATH];
//    lstrcpyn(szTemp, pszCmdLine, ARRAYSIZE(szTemp));
//    PathRemoveArgs(szTemp);
//    PathUnquoteSpaces(szTemp);

    PathToAppPathKey(pszCmdLine, szRegKeyName, ARRAYSIZE(szRegKeyName));
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szRegKeyName, 0L, KEY_QUERY_VALUE, &hKeyAppPaths) == ERROR_SUCCESS)
    {
        bRet = RegQueryValueEx(hKeyAppPaths, TEXT("UseURL"), NULL, NULL, NULL, NULL) == ERROR_SUCCESS;
        RegCloseKey(hKeyAppPaths);
    }

    return bRet;
}


// thread reference count object, this uses SHSetThreadRef()to let other code
// in this process hold a reference to this main thread, and thus the main thread in this process

class CRefThread : public IUnknown
{
public:
    // IUnknown
    virtual STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
    virtual STDMETHODIMP_(ULONG) AddRef(void);
    virtual STDMETHODIMP_(ULONG) Release(void);

    CRefThread(LONG *pcRef);

private:
    ~CRefThread();

    LONG *_pcRef;
    UINT _idThread;
};


CRefThread::CRefThread(LONG *pcRef) 
{
    _idThread = GetCurrentThreadId();
    _pcRef = pcRef;
    *_pcRef = 1;

}

//
//  Note that this code tightens but does not close a race window.
//  Although we nuke the process reference, the class factory for
//  the web browser has yet to be deregistered, so if somebody decides
//  to create one, our class factory will wake up and create a
//  shell folder, which will flake out because it can't get a
//  process reference.
//
CRefThread::~CRefThread() 
{
    // Avoid re-entrancy during destruction
    *_pcRef = 1000;

    // If we are the process reference, then revoke the process reference
    // since we're going away.

    IUnknown *punk;
    SHGetInstanceExplorer(&punk);
    if (punk == this)
        SHSetInstanceExplorer(NULL);
    ATOMICRELEASE(punk);

    // Nobody should've rescued our reference
    ASSERT(*_pcRef == 1000);
    *_pcRef = 0;

    // get the other thread out of WaitMessage() or GetMessage()
    PostThreadMessage(_idThread, WM_NULL, 0, 0);
}


HRESULT CRefThread::QueryInterface(REFIID riid, void **ppvObj)
{
    static const QITAB qit[] = { { 0 }, };
    return QISearch(this, qit, riid, ppvObj);
}

ULONG CRefThread::AddRef()
{
    return InterlockedIncrement(_pcRef);
}

ULONG CRefThread::Release()
{
    if (InterlockedDecrement(_pcRef))
        return *_pcRef;

    delete this;
    return 0;
}

STDAPI SHCreateThreadRef(LONG *pcRef, IUnknown **ppunk)
{
    *ppunk = new CRefThread(pcRef);
    if (*ppunk)
        return S_OK;

    *pcRef = 0;
    *ppunk = NULL;
    return E_OUTOFMEMORY;
}

//
// Returns the cache file associated with a URL.  For file: urls, the associated
// disk file is returned.  Not that we don't use URLDownloadToCacheFile because 
// it causes another I-M-S GET to to be sent to the server
//
HRESULT URLToCacheFile
(
    LPCWSTR pszUrl,
    LPWSTR pszFile,
    int    cchFile
)
{
    HRESULT hr = E_FAIL;
    DWORD dwScheme = GetUrlScheme(pszUrl);
    if (URL_SCHEME_FILE == dwScheme)
    {
        ULONG cch = cchFile;
        hr = PathCreateFromUrl(pszUrl, pszFile, &cch, 0);
    }
    else
    {
        // bug 73386 - GetUrlCacheEntryInfoExW fails to find entries if there is an anchor
        // so we have to whack it off.
        //
        // We should really fix GetUrlCacheEntryInfoExW instead, but apparently 
        // this is risky for 5.x
        //
        WCHAR szUrlBuf[MAX_URL_STRING];
        if (URL_SCHEME_HTTP == dwScheme || URL_SCHEME_HTTPS == dwScheme)
        {
            LPWSTR pszAnchor = StrChr(pszUrl, L'#');
            if (pszAnchor)
            {
                int cch = (int)min((int)(pszAnchor - pszUrl + 1), ARRAYSIZE(szUrlBuf));
                StrCpyN(szUrlBuf, pszUrl, cch);
                pszUrl = szUrlBuf;
            }
        }

        char szBuf[1024];
        LPINTERNET_CACHE_ENTRY_INFOW pCE = (LPINTERNET_CACHE_ENTRY_INFOW)szBuf;
        DWORD dwEntrySize = sizeof(szBuf);

        BOOL fGotCacheInfo = GetUrlCacheEntryInfoExW(pszUrl, pCE, &dwEntrySize, NULL, NULL, NULL, 0);
        if (!fGotCacheInfo)
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                // We guessed too small for the buffer so allocate the correct size & retry
                pCE = (LPINTERNET_CACHE_ENTRY_INFOW)LocalAlloc(LPTR, dwEntrySize);
                if (pCE)
                {
                    fGotCacheInfo = GetUrlCacheEntryInfoEx(pszUrl, pCE, &dwEntrySize, NULL, NULL, NULL, 0);
                }
            }
            else 
            {
                // Retry using UTF8 encoding 
                //
                // This fix belongs in GetUrlCacheEntryInfoEx (StevePro 01/19/99)
                //
                char szUrl[MAX_URL_STRING];
                if (SHUnicodeToAnsiCP(CP_UTF8, pszUrl, szUrl, ARRAYSIZE(szUrl)))
                {
                    szUrl[ARRAYSIZE(szUrl)-1] = '\0';   // paranoia

                    // UrlEscapeA Internally converts to unicode which messes up utf8.  So we
                    // copy the string to a WCHAR buffer without coverting and call the unicode version.
                    // Yuk!
                    WCHAR wzUrl[ARRAYSIZE(szUrl)];
                    char* psz = szUrl;
                    WCHAR* pwz = wzUrl;

                    while (*psz!= NULL)
                    {
                        *pwz++ = ((WCHAR)*psz++) & 0xff;
                    }
                    *pwz = L'\0';

                    ULONG cch = ARRAYSIZE(wzUrl);
                    UrlEscapeW(wzUrl, wzUrl, &cch, /*URL_ESCAPE_PERCENT*/0);

                    psz = szUrl;
                    pwz = wzUrl;
                    while (*pwz!= NULL) 
                    {
                        *psz++ = (char)LOWORD(*pwz++);
                    }
                    *psz = '\0';

                    LPINTERNET_CACHE_ENTRY_INFOA pCEA = (LPINTERNET_CACHE_ENTRY_INFOA)szBuf;
                    dwEntrySize = sizeof(szBuf);

                    BOOL fUtf8Worked = GetUrlCacheEntryInfoExA(szUrl, pCEA, &dwEntrySize, NULL, NULL, NULL, 0);
                    if (!fUtf8Worked)
                    {
                        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                        {
                            // We guessed too small for the buffer so allocate the correct size & retry
                            pCEA = (LPINTERNET_CACHE_ENTRY_INFOA)LocalAlloc(LPTR, dwEntrySize);
                            if (pCEA)
                            {
                                fUtf8Worked = GetUrlCacheEntryInfoExA(szUrl, pCEA, &dwEntrySize, NULL, NULL, NULL, 0);
                            }
                        }
                    }

                    if (fUtf8Worked)
                    {
                        SHAnsiToUnicode(pCEA->lpszLocalFileName, pszFile, cchFile);
                        hr = S_OK;
                    }

                    if ((char *)pCEA != szBuf)
                    {
                        LocalFree((HLOCAL)pCEA);
                    }
                }
            }
        }

        if (fGotCacheInfo)
        {
            StrCpyN(pszFile, pCE->lpszLocalFileName, cchFile);
            hr = S_OK;
        }

        // Free our GetUrlCacheEntryInfo buffer if we allocated one
        if ((char *)pCE != szBuf)
        {
            LocalFree((HLOCAL)pCE);
        }
    }
    return hr;
}

#ifdef DEBUG
void DebugDumpPidl(DWORD dwDumpFlag, LPTSTR pszOutputString, LPCITEMIDLIST pidl)
{
    if (g_dwDumpFlags & dwDumpFlag)
    {
        TCHAR szPath[MAX_PATH];
        LPTSTR lpsz;
        if (pidl)  
        {
            lpsz = szPath;
            SHGetPathFromIDList(pidl, szPath);
        } 
        else 
        {
            lpsz = TEXT("(NULL)");
        }
        TraceMsg(TF_ALWAYS, "%s: \"%s\"", pszOutputString, lpsz);
    }
}
#endif

// Variable argument version that ultimately call FormatMessageLiteW
BOOL __cdecl _FormatMessage(LPCWSTR szTemplate, LPWSTR szBuf, UINT cchBuf, ...)
{
    BOOL fRet;
    va_list ArgList;
    va_start(ArgList, cchBuf);

    fRet = FormatMessage(FORMAT_MESSAGE_FROM_STRING, szTemplate, 0, 0, szBuf, cchBuf, &ArgList);

    va_end(ArgList);
    return fRet;
}


// [msadek], On win9x we get the message thru a chain from explorer /iexplore (ANSI app.).
// and pass it to comctl32 (Unicode) so it will fail to match the hot key.
// the system sends the message with ANSI char and we treated it as Unicode.
// It looks like noone is affected with this bug (US, FE) since they have hot keys always in Latin.
// Bidi platforms are affected since they do have hot keys in native language.

WPARAM AnsiWparamToUnicode(WPARAM wParam)
{
    char szCh[2];
    WCHAR wszCh[2];
    szCh[0] = (BYTE)wParam;
    szCh[1] = '\0';

    if (MultiByteToWideChar(CP_ACP, 0, (LPCSTR)szCh, ARRAYSIZE(szCh),
                                   wszCh, ARRAYSIZE(wszCh)))
    {
        memcpy(&wParam, wszCh, sizeof(WCHAR));
    }

    return wParam;
}

void SHOutlineRect(HDC hdc, const RECT* prc, COLORREF cr)
{
    RECT rc;
    COLORREF clrSave = SetBkColor(hdc, cr);
    
    //top
    rc.left = prc->left;
    rc.top = prc->top;
    rc.right = prc->right;
    rc.bottom = prc->top + 1;
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

    //left
    rc.left = prc->left;
    rc.top = prc->top;
    rc.right = prc->left + 1;
    rc.bottom = prc->bottom;
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

    //right
    rc.left = prc->right - 1;
    rc.top = prc->top;
    rc.right = prc->right;
    rc.bottom = prc->bottom;
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

    // bottom
    rc.left = prc->left;
    rc.top = prc->bottom - 1;
    rc.right = prc->right;
    rc.bottom = prc->bottom;
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

    SetBkColor(hdc, clrSave);
}

HMONITOR GetPrimaryMonitor()
{
    POINT pt = {0,0};
    return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY); 
}

// Gets the Monitor's bounding or work rectangle, if the hMon is bad, return
// the primary monitor's bounding rectangle. 
BOOL GetMonitorRects(HMONITOR hMon, LPRECT prc, BOOL bWork)
{
    MONITORINFO mi; 
    mi.cbSize = sizeof(mi);
    if (hMon && GetMonitorInfo(hMon, &mi))
    {
        if (!prc)
            return TRUE;
        
        else if (bWork)
            CopyRect(prc, &mi.rcWork);
        else 
            CopyRect(prc, &mi.rcMonitor);
        
        return TRUE;
    }
    
    if (prc)
        SetRect(prc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//  Utils to load background bitmap for toolbars etc.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



//------------------------------------------------------------------------
// determine source name for bitmap, sift thru IE history....
HRESULT _GetBackBitmapLocation(LPTSTR psz, BOOL fInternet)
{
    HRESULT hres = E_FAIL;
    DWORD dwType;
    DWORD dwcbData;
    static const TCHAR c_szRegKeyCoolbar[] = TSZIEPATH TEXT("\\Toolbar");

    // IE4 shipped back bitmap customization affecting both browser and shell.
    // IE5 wants these to be separate customizations.  But in the roaming
    // case a customized IE4 customer shouldn't lose customization when going
    // to the IE5 machine.  So we might need to check twice:
    //
    if (fInternet)
    {
        // Try the IE5 internet location.
        dwcbData = MAX_PATH * sizeof(TCHAR);
        hres = SHGetValue(HKEY_CURRENT_USER, c_szRegKeyCoolbar, TEXT("BackBitmapIE5"), &dwType, psz, &dwcbData);
    }
    else
    {
        // Try the NT5 shell location.
        dwcbData = MAX_PATH * sizeof(TCHAR);
        hres = SHGetValue(HKEY_CURRENT_USER, c_szRegKeyCoolbar, TEXT("BackBitmapShell"), &dwType, psz, &dwcbData);
    }
    if (ERROR_SUCCESS != hres)
    {
        // Try the old combined internet/shell location
        dwcbData = MAX_PATH * sizeof(TCHAR);
        hres = SHGetValue(HKEY_CURRENT_USER, c_szRegKeyCoolbar, TEXT("BackBitmap"), &dwType, psz, &dwcbData);
    }

    return hres;
}


//------------------------------------------------------------------------
//  determine background settings and source for toolbar,
//  load bitmap (file/resource) and update cache
HBITMAP LoadToolbarBackBmp(LPTSTR * ppszBitmap, BMPCACHE * pbmpCache, BOOL fInternet)
{
    HIGHCONTRAST    hc;
    HBITMAP     hbmp = pbmpCache->hbmp;
    COLORREF    cr3D = GetSysColor(COLOR_3DFACE);
    TCHAR       szScratch[MAX_PATH];
    LPTSTR      pszBitmap = NULL;
    BOOL        fBitmapInvalid = FALSE;


    ENTERCRITICAL;

    // If the stashed hbmp's cr3D color changed, we need to mark invalid
    if (pbmpCache->hbmp && pbmpCache->cr3D != cr3D)
        fBitmapInvalid = TRUE;

    // get the location spec for the bitmap
    hc.cbSize = sizeof(HIGHCONTRAST);
    if ((SystemParametersInfoA(SPI_GETHIGHCONTRAST, hc.cbSize, (LPVOID) &hc, FALSE)) &&
        (hc.dwFlags & HCF_HIGHCONTRASTON))
    {
        // we have no bitmap in high contrast
    }
    else if (SUCCEEDED(_GetBackBitmapLocation(szScratch, fInternet)))
    {
        pszBitmap = szScratch;
    }

    // if they are removing the bitmap, we need to mark invalid
    if (!pszBitmap && *ppszBitmap)
        fBitmapInvalid = TRUE;

    // or it's location has been changed, we need to mark invalid
    if (pszBitmap && (!*ppszBitmap || lstrcmpi(pszBitmap, *ppszBitmap)))
        fBitmapInvalid = TRUE;

    if (fBitmapInvalid)
    {
        TraceMsg(DM_ITBAR, "LoadToolbarBackBmp: Loading Background Bitmap");

        Str_SetPtr(ppszBitmap, pszBitmap);

        hbmp=NULL;
        if (*ppszBitmap)
        {
            if ((*ppszBitmap)[0])
            {
                hbmp = (HBITMAP) LoadImage(NULL, szScratch, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION | LR_LOADFROMFILE | LR_LOADMAP3DCOLORS );
            }

            if (!hbmp)
            {
#ifdef OLD_SWIRLY_BACKDROP
                if (SHGetCurColorRes() <= 8)
                    hbmp = (HBITMAP) LoadImage(HINST_THISDLL, MAKEINTRESOURCE(IDB_BACK), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS );
#endif
            }
        }

#ifdef OLD_LEGACY_BAD_COLOUR_CODE
        if (hbmp)
        {
            // mapping needed ?
            // DONTWORRYABOUTTHIS: this will be removed as soon as I get the new backdrop....
            if ( /* cr3D != RGB(192,192,192) */ FALSE)
            {
                RGBQUAD     rgbTable[256];
                RGBQUAD     rgbFace;
                HDC         dc;
                HBITMAP     hbmSave;
                UINT        i;
                UINT        n;

                dc = CreateCompatibleDC(NULL);
                hbmSave = (HBITMAP)SelectObject(dc, hbmp);
                n = GetDIBColorTable(dc, 0, 256, rgbTable);

                rgbFace.rgbRed   = GetRValue(cr3D);
                rgbFace.rgbGreen = GetGValue(cr3D);
                rgbFace.rgbBlue  = GetBValue(cr3D);

                for (i = 0; i < n; i++)
                {
                    if ( rgbTable[i].rgbRed == 192 && rgbTable[i].rgbGreen == 192 && rgbTable[i].rgbBlue == 192 )
                    {
                        rgbTable[i] = rgbFace;
                    }
                    else
                    {
                        rgbTable[i].rgbRed   = (rgbTable[i].rgbRed   * rgbFace.rgbRed  ) / 192;
                        rgbTable[i].rgbGreen = (rgbTable[i].rgbGreen * rgbFace.rgbGreen) / 192;
                        rgbTable[i].rgbBlue  = (rgbTable[i].rgbBlue  * rgbFace.rgbBlue ) / 192;
                    }
                }

                SetDIBColorTable(dc, 0, n, rgbTable);
                SelectObject(dc, hbmSave);
                DeleteDC(dc);
            }
        }
#endif

        if (pbmpCache->hbmp)
            DeleteObject(pbmpCache->hbmp);
        pbmpCache->hbmp = hbmp;
        pbmpCache->cr3D = cr3D;
    }

    LEAVECRITICAL;

    return hbmp;
}


VOID StripDecorations(PTSTR pszTitle, BOOL fStripAmp)
{
    LPTSTR  pszCleaned = pszTitle;    // work in-place
    LPCTSTR psz = pszTitle;
    while (*psz && (*psz != TEXT('\t')))
    {
        if (*psz != TEXT('&') || !fStripAmp)
        {
            *pszCleaned = *psz;
            pszCleaned++;
        }
        psz++;
    }
    *pszCleaned = TEXT('\0');
}

//------------------------------------------------------------------------
LPCTSTR UnescapeDoubleAmpersand(LPTSTR pszTitle)
{
    LPTSTR  pszCleaned = pszTitle;    // work in-place
    LPCTSTR psz = pszTitle;
    bool fEscapedAmp = false;
    while (*psz)
    {
        if (*psz != TEXT('&') || fEscapedAmp)
        {
            // copy character
            *pszCleaned = *psz;
            pszCleaned++;
            fEscapedAmp = false;
        }
        else
        {
            LPCTSTR pszNext = psz + 1;
            if (pszNext && (*pszNext == TEXT('&'))) {
                fEscapedAmp = true; // keep next ampersand
            }
        }
        psz++;
    }
    *pszCleaned = TEXT('\0');
    return pszTitle;
}

UINT MapClsidToID(REFCLSID rclsid)
{
    UINT nCmdID;

    nCmdID = 0;

    if (IsEqualCLSID(CLSID_SearchBand, rclsid))
        nCmdID = FCIDM_VBBSEARCHBAND;
    else if (IsEqualCLSID(CLSID_FavBand, rclsid))
        nCmdID = FCIDM_VBBFAVORITESBAND;
    else if (IsEqualCLSID(CLSID_HistBand, rclsid))
        nCmdID = FCIDM_VBBHISTORYBAND;
    else if (IsEqualCLSID(CLSID_ExplorerBand, rclsid))
        nCmdID = FCIDM_VBBEXPLORERBAND;
    else if (IsEqualCLSID(CLSID_FileSearchBand, rclsid))
        nCmdID = FCIDM_VBBSEARCHBAND;

    return nCmdID;        
}


// Create mask from given bitmap, use color at pixel (x/y) as transparent color
HBITMAP CreateMaskBitmap(HDC hdc, int x, int y, HBITMAP hbmpImage)
{
    ASSERT(hbmpImage);
    BITMAP bm;
    if (::GetObject(hbmpImage, sizeof(BITMAP), &bm) != sizeof(BITMAP)) {
        return FALSE;
    }

    HDC hdcImg = NULL;
    HDC hdcMask = NULL;
    HBITMAP hbmpMask = NULL;
    HBITMAP hbmpOldImg = NULL;
    HBITMAP hbmpOldMsk = NULL;
    COLORREF clrTransparent = 0;

    hdcImg = ::CreateCompatibleDC(hdc);
    if (hdcImg == NULL)     goto _CMBcleanup;
    hdcMask = ::CreateCompatibleDC(hdc);
    if (hdcMask == NULL)    goto _CMBcleanup;

    hbmpMask = ::CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
    if (hbmpMask == NULL)   goto _CMBcleanup;
    hbmpOldImg = (HBITMAP) ::SelectObject(hdcImg, hbmpImage);
    hbmpOldMsk = (HBITMAP) ::SelectObject(hdcMask, hbmpMask);

    clrTransparent = ::GetPixel(hdcImg, 0, 0);
    ::SetBkColor(hdcImg, clrTransparent);
    ::BitBlt(hdcMask, 0, 0, bm.bmWidth, bm.bmHeight, hdcImg, 0, 0, SRCCOPY);

_CMBcleanup:
    if (hbmpOldImg && hdcImg)
        SelectObject(hdcImg, hbmpOldImg);
    if (hdcImg)
        DeleteDC(hdcImg);
    if (hbmpOldMsk && hdcMask)
        SelectObject(hdcMask, hbmpOldMsk);
    if (hdcMask)
        DeleteDC(hdcMask);

    return hbmpMask;
}

// draw bitmap transparently; on Win2K and up, one could use MaskBlt()
BOOL DrawTransparentBitmapPart(HDC hdc, int x, int y, int dx, int dy, HBITMAP hbmpImage, HBITMAP hbmpMask)
{
    ASSERT(hbmpImage);
    BITMAP bm;
    if (::GetObject(hbmpImage, sizeof(BITMAP), &bm) != sizeof(BITMAP)) {
        return FALSE;
    }

    HBITMAP hbmpTmpMask = NULL;
    // create temporary mask bitmap if none supplied
    if (hbmpMask == NULL) {
        hbmpMask = hbmpTmpMask = CreateMaskBitmap(hdc, 0, 0, hbmpImage);
    }
    if (hbmpMask == NULL) {
        return FALSE;
    }

    HDC hdcOffScr = NULL;
    HBITMAP hbmOffScr = NULL;
    HBITMAP hbmOldOffScr = NULL;
    HDC hdcImage = NULL;
    HBITMAP hbmOldImage = NULL;
    HDC hdcMask = NULL;
    HBITMAP hbmOldMask = NULL;

    // draw.to offscreen bitmap
    hdcOffScr = ::CreateCompatibleDC(hdc);
    if (hdcOffScr == NULL)      goto _DTBcleanup;
    hbmOffScr = ::CreateBitmap(dx, dy,GetDeviceCaps(hdc, PLANES),
                               (BYTE)GetDeviceCaps(hdc, BITSPIXEL), NULL);
    if (hbmOffScr == NULL)      goto _DTBcleanup;
    hbmOldOffScr = (HBITMAP)::SelectObject(hdcOffScr, hbmOffScr);

    // Copy the image of the destination rectangle to the
    // off-screen buffer DC, so we can play with it.
    ::BitBlt(hdcOffScr, 0, 0, dx, dy, hdc, x, y, SRCCOPY);

    // prepare DCs for both image and mask
    hdcImage = ::CreateCompatibleDC(hdc); 
    if (hdcImage == NULL)       goto _DTBcleanup;
    hbmOldImage = (HBITMAP)::SelectObject(hdcImage, hbmpImage);
    hdcMask = ::CreateCompatibleDC(hdc);
    if (hdcMask == NULL)        goto _DTBcleanup;
    hbmOldMask = (HBITMAP)::SelectObject(hdcMask, hbmpMask);

    ::SetBkColor(hdcOffScr, RGB(255,255,255));
    ::BitBlt(hdcOffScr, 0, 0, dx, dy, hdcImage, 0, 0, SRCINVERT);
    ::BitBlt(hdcOffScr, 0, 0, dx, dy, hdcMask,  0, 0, SRCAND);
    ::BitBlt(hdcOffScr, 0, 0, dx, dy, hdcImage, 0, 0, SRCINVERT);

    // Copy the resultant image back to the screen DC.
    ::BitBlt(hdc,       x, y, dx, dy, hdcOffScr, 0, 0, SRCCOPY);

_DTBcleanup:
    if (hdcOffScr && hbmOldOffScr)
        ::SelectObject(hdcOffScr, hbmOldOffScr);
    if (hdcOffScr)
        ::DeleteDC(hdcOffScr);
    if (hbmOffScr)
        ::DeleteObject(hbmOffScr);

    if (hdcImage && hbmOldImage)
        ::SelectObject(hdcImage, hbmOldImage);
    if (hdcImage)
        ::DeleteDC(hdcImage);

    if (hdcMask && hbmOldMask)
        ::SelectObject(hdcMask, hbmOldMask);
    if (hdcMask)
        ::DeleteDC(hdcMask);
    if (hbmpTmpMask)
        ::DeleteObject(hbmpTmpMask);

    return TRUE;
}

// draw bitmap transparently; on Win2K and up, one could use MaskBlt()
BOOL DrawTransparentBitmap(HDC hdc, int x, int y, HBITMAP hbmpImage, HBITMAP hbmpMask)
{
    ASSERT(hbmpImage);
    BITMAP bm;
    if (::GetObject(hbmpImage, sizeof(BITMAP), &bm) != sizeof(BITMAP)) {
        return FALSE;
    }

    return DrawTransparentBitmapPart(hdc, x, y, bm.bmWidth, bm.bmHeight, hbmpImage, hbmpMask);
}

//------------------------------------------------------------------------
BOOL
    DrawAlphaBitmap(HDC hdc, int x, int y, int dx, int dy, HBITMAP hbmpImage)
{
    BLENDFUNCTION bf = {0};
    HDC hdcImage = ::CreateCompatibleDC(hdc);
    if (hdcImage == NULL) {
        return false;
    }
    HBITMAP hbmOldImage = (HBITMAP)::SelectObject(hdcImage, hbmpImage);

    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;
    AlphaBlend(hdc, x, y, dx, dy, hdcImage, 0, 0, dx, dy, bf);
    
    if (hbmOldImage) {
        SelectObject(hdcImage, hbmOldImage);
    }
    DESTROY_OBJ_WITH_HANDLE(hdcImage, DeleteObject);

    return true;
}


STDAPI_(IDeskBand *) FindBandByClsidBS(IBandSite *pbs, REFCLSID clsidToFind)
{
    DWORD dwBandID;
    for (int i = 0; SUCCEEDED(pbs->EnumBands(i, &dwBandID)); i++) 
    {
        IDeskBand *pstb;
        HRESULT hr = pbs->QueryBand(dwBandID, &pstb, NULL, NULL, 0);
        if (SUCCEEDED(hr)) 
        {
            CLSID clsid;

            hr = IUnknown_GetClassID(pstb, &clsid);
            if (SUCCEEDED(hr) && IsEqualGUID(clsidToFind, clsid)) 
            {
                return pstb;
            }
            pstb->Release();
        }
    }

    return NULL;
}

HIMAGELIST CreateImageList(HINSTANCE hi, LPCTSTR lpbmp, int cx, int cGrow, COLORREF crMask,
                           UINT uType, UINT uFlags, BOOL bUseNewMirroringSupport)
{
    HBITMAP hbmImage;
    HIMAGELIST piml = NULL;
    BITMAP bm;
    int cy, cInitial;
    UINT flags;

    hbmImage = (HBITMAP)LoadImage(hi, lpbmp, uType, 0, 0, uFlags);
    if (hbmImage && (sizeof(bm) == GetObject(hbmImage, sizeof(bm), &bm)))
    {
        // If cx is not stated assume it is the same as cy.
        // ASSERT(cx);
        cy = bm.bmHeight;

        if (cx == 0)
            cx = cy;

        cInitial = bm.bmWidth / cx;

        ENTERCRITICAL;

        if (bUseNewMirroringSupport)
        {
            flags = ILC_MIRROR | PrivateILC_PERITEMMIRROR;
        }
        else
        {
            flags = 0;
        }

        if (crMask != CLR_NONE)
            flags |= ILC_MASK;
        if (bm.bmBits)
            flags |= (bm.bmBitsPixel & ILC_COLORMASK);

        piml = ImageList_Create(cx, cy, flags, cInitial, cGrow);
        if (piml)
        {
            int added;

            if (crMask == CLR_NONE)
                added = ImageList_Add(piml, hbmImage, NULL);
            else
                added = ImageList_AddMasked(piml, hbmImage, crMask);

            if (added < 0)
            {
                ImageList_Destroy(piml);
                piml = NULL;
            }
        }
        LEAVECRITICAL;
    }

    if (hbmImage)
        DeleteObject(hbmImage);

    return piml;
}
