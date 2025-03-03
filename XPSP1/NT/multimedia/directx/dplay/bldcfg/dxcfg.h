/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dpvcfg.h
 *  Content:    Constant declarations for standalone DLL
 *  History:
 
 * Date     By      Reason
 * ====     ======= ========================================================
 * 02/25/00 rodtoll Created
 ***************************************************************************/

#ifndef __DXCFG_H
#define __DXCFG_H

// DEFAULT DIRECTPLAYVOICE CONFIG FILE
//
// This file is for the default build.
//

#define DPVOICE_REGISTRY_BASE           L"SOFTWARE\\MICROSOFT\\DIRECTPLAY\\VOICE"
#define DPVOICE_REGISTRY_CP             L"\\Compression Providers"
#define DPVOICE_REGISTRY_DPVACM         L"\\DPVACM"
#define DPVOICE_REGISTRY_DPVVOX         L"\\DPVVOX"
#define DPVOICE_REGISTRY_AGC            L"\\AGC"
#define DPVOICE_REGISTRY_AUDIOCONFIG   L"\\AudioConfig"
#define DPVOICE_CLSID_DPVOICE           CLSID_DirectPlayVoice   
#define DPVOICE_CLSID_DPVVOX            CLSID_DPVCPVOX
#define DPVOICE_CLSID_DPVVOX_CONVERTER  CLSID_DPVCPVOX_CONVERTER
#define DPVOICE_CLSID_DPVACM            CLSID_DPVCPACM
#define DPVOICE_CLSID_DPVACM_CONVERTER	CLSID_DPVCPACM_CONVERTER
#define DPLAY_CLSID_DPLAY               CLSID_DirectPlay
#define DPLAY_CLSID_DPLOBBY             CLSID_DirectPlayLobby

// Unicode filenames (for resource files)
#define DPVOICE_FILENAME_DPVOICE        L"dpvoice.dll"
#define DPVOICE_FILENAME_DPVSETUP       L"dpvsetup.exe"
#define DPVOICE_FILENAME_RES             L"dpvoice.dll"
#define DPVOICE_FILENAME_DPVVOX         L"dpvvox.dll"
#define DPVOICE_FILENAME_DPVACM         L"dpvacm.dll"
#define DPLAY_FILENAME_DPLAYX	        L"dplayx.dll"
#define DPVOICE_FILENAME_DPVHELP        L"dpvhelp.exe"
#define DPLAY_FILENAME_DPWSOCKX			L"dpwsockx.dll"

// ANSI filenames (for resource files)
#define DPVOICE_FILENAME_DPVOICE_A      "dpvoice.dll"
#define DPVOICE_FILENAME_DPVSETUP_A     "dpvsetup.exe"
#define DPVOICE_FILENAME_RES_A           "dpvoice.dll"
#define DPVOICE_FILENAME_DPVVOX_A       "dpvvox.dll"
#define DPVOICE_FILENAME_DPVACM_A       "dpvacm.dll"
#define DPLAY_FILENAME_DPLAYX_A	        "dplayx.dll"
#define DPVOICE_FILENAME_DPVHELP_A      "dpvhelp.exe"
#define DPLAY_FILENAME_DPWSOCKX_A		"dpwsockx.dll"

#define DPVOICE_COMMANDLINE_PRIORITY    _T( DPVOICE_FILENAME_DPVSETUP_A ) _T( " -p")
#define DPVOICE_COMMANDLINE_FULLDUPLEX  _T( DPVOICE_FILENAME_DPVSETUP_A ) _T(" -f")

// Version Info
#define DPVOICE_VERINFO_DPVOICE          "Microsoft DirectPlay Voice"
#define DPVOICE_VERINFO_DPVOICE_DEBUG   DPVOICE_VERINFO_DPVOICE "Debug\0"
#define DPVOICE_VERINFO_DPVOICE_RETAIL  DPVOICE_VERINFO_DPVOICE "\0"
#define DPVOICE_VERINFO_DPVOICE_WINNT   DPVOICE_VERINFO_DPVOICE
#define DPVOICE_VERNAME_DPVOICE_WIN9X   DPVOICE_FILENAME_DPVOICE_A "\0"
#define DPVOICE_VERNAME_DPVOICE_WINNT   DPVOICE_FILENAME_DPVOICE_A

// dpvvox
#define DPVOICE_VERINFO_DPVVOX           "Microsoft DirectPlay Voice Voxware Provider"
#define DPVOICE_VERINFO_DPVVOX_DEBUG    DPVOICE_VERINFO_DPVVOX " Debug\0"
#define DPVOICE_VERINFO_DPVVOX_RETAIL   DPVOICE_VERINFO_DPVVOX "\0"
#define DPVOICE_VERINFO_DPVVOX_WINNT    DPVOICE_VERINFO_DPVVOX
#define DPVOICE_VERNAME_DPVVOX_WIN9X   DPVOICE_FILENAME_DPVVOX_A "\0"
#define DPVOICE_VERNAME_DPVVOX_WINNT   DPVOICE_FILENAME_DPVVOX_A

// dpvacm
#define DPVOICE_VERINFO_DPVACM          "Microsoft DirectPlay Voice ACM Provider"
#define DPVOICE_VERINFO_DPVACM_DEBUG    DPVOICE_VERINFO_DPVACM " Debug\0"
#define DPVOICE_VERINFO_DPVACM_RETAIL   DPVOICE_VERINFO_DPVACM "\0"
#define DPVOICE_VERINFO_DPVACM_WINNT    DPVOICE_VERINFO_DPVACM
#define DPVOICE_VERNAME_DPVACM_WIN9X   DPVOICE_FILENAME_DPVACM_A "\0"
#define DPVOICE_VERNAME_DPVACM_WINNT   DPVOICE_FILENAME_DPVACM_A

// dpvsetup
#define DPVOICE_VERINFO_DPVSETUP         "Microsoft DirectPlay Voice Test"
#define DPVOICE_VERINFO_DPVSETUP_DEBUG  DPVOICE_VERINFO_DPVSETUP "Debug\0"
#define DPVOICE_VERINFO_DPVSETUP_RETAIL DPVOICE_VERINFO_DPVSETUP "\0"
#define DPVOICE_VERINFO_DPVSETUP_WINNT  DPVOICE_VERINFO_DPVSETUP
#define DPVOICE_VERNAME_DPVSETUP_WIN9X   DPVOICE_FILENAME_DPVSETUP_A "\0"
#define DPVOICE_VERNAME_DPVSETUP_WINNT   DPVOICE_FILENAME_DPVSETUP_A

// dplayx
#define DPLAY_VERINFO_DPLAY         	"Microsoft DirectPlay"
#define DPLAY_VERINFO_DPLAY_DEBUG  	DPLAY_VERINFO_DPLAY "Debug\0"
#define DPLAY_VERINFO_DPLAY_RETAIL 	DPLAY_VERINFO_DPLAY "\0"
#define DPLAY_VERINFO_DPLAY_WINNT  	DPLAY_VERINFO_DPLAY
#define DPLAY_VERNAME_DPLAY_WIN9X   	DPLAY_FILENAME_DPLAYX_A "\0"
#define DPLAY_VERNAME_DPLAY_WINNT   	DPLAY_FILENAME_DPLAYX_A

// dplayx
#define DPLAY_VERINFO_DPWSOCKX         	"Internet TCP/IP and IPX Connection For DirectPlay"
#define DPLAY_VERINFO_DPWSOCKX_DEBUG  	DPLAY_VERINFO_DPWSOCKX "Debug\0"
#define DPLAY_VERINFO_DPWSOCKX_RETAIL 	DPLAY_VERINFO_DPWSOCKX "\0"
#define DPLAY_VERINFO_DPWSOCKX_WINNT  	DPLAY_VERINFO_DPWSOCKX
#define DPLAY_VERNAME_DPWSOCKX_WIN9X   	DPLAY_FILENAME_DPWSOCKX_A "\0"
#define DPLAY_VERNAME_DPWSOCKX_WINNT   	DPLAY_FILENAME_DPWSOCKX_A

// dpvhelp
#define DPVOICE_VERINFO_DPVHELP         "Microsoft DirectPlay Voice"
#define DPVOICE_VERINFO_DPVHELP_DEBUG  DPVOICE_VERINFO_DPVHELP "Debug\0"
#define DPVOICE_VERINFO_DPVHELP_RETAIL DPVOICE_VERINFO_DPVHELP "\0"
#define DPVOICE_VERINFO_DPVHELP_WINNT  DPVOICE_VERINFO_DPVHELP
#define DPVOICE_VERNAME_DPVHELP_WIN9X  DPVOICE_FILENAME_DPVHELP_A "\0"
#define DPVOICE_VERNAME_DPVHELP_WINNT  DPVOICE_FILENAME_DPVHELP_A


#endif

