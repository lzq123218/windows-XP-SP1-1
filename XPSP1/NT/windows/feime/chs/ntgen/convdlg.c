
/*************************************************
 *  convdlg.c                                    *
 *                                               *
 *  Copyright (C) 1995-1999 Microsoft Inc.       *
 *                                               *
 *************************************************/

#include <string.h>
#include "prop.h"
#include "upimeres.h"
#include <regstr.h>
#include <htmlhelp.h>

#ifdef UNICODE
extern TCHAR Title[];
extern TCHAR szVer[];
#else
extern BYTE Title[];
extern BYTE szVer[];
#endif

/*****************************************************************************

  FUNCTION: ConvDialogProc(HWND, UINT, WPARAM, LPARAM)

  PURPOSE:  Processes messages for "conv" property sheet.

  PARAMETERS:
    hdlg - window handle of the property sheet
    wMessage - type of message
    wparam - message-specific information
    lparam - message-specific information

  RETURN VALUE:
    TRUE - message handled
    FALSE - message not handled

  HISTORY:
    04-18-95 Yehfew Tie  Created.
 ****************************************************************************/

INT_PTR  CALLBACK ConvDialogProc(HWND hdlg,
                              UINT uMessage,
                              WPARAM wparam,
                              LPARAM lparam)
{
    LPNMHDR      lpnmhdr;
	static DWORD dwDesOffset,dwDesLen;
	static DWORD dwRuleOffset,dwRuleLen;
	static TCHAR szMbName[MAX_PATH];
	static TCHAR szSrcName[MAX_PATH];
	static TCHAR _szStr[MAX_PATH];
    static BOOL  bModify;
	LPRULE       lpRule;
	int 		 i;
    static DESCRIPTION Descript;


    switch (uMessage)
    {
	    case WM_INITDIALOG:
		{	

			//install conv sub class
		    lpConvProc = (FARPROC)SetWindowLongPtr(GetDlgItem(hdlg,IDC_LIST),
		    							GWLP_WNDPROC,(LONG_PTR)ConvEditProc);
		    SendDlgItemMessage(hdlg,IDC_IMENAME,EM_LIMITTEXT,24,0L);
		    SendDlgItemMessage(hdlg,IDC_USEDCODE,EM_LIMITTEXT,MAXUSEDCODES,0L);
			SetConvDisable(hdlg);
			PropSheet_CancelToClose(GetParent(hdlg));
            break;
		}
		
		
        case WM_DESTROY:
			if(hRule)
   			   GlobalFree(hRule);
			if(hEncode)
			   GlobalFree(hEncode);
			break;

		case WM_NOTIFY:
            lpnmhdr = (NMHDR FAR *)lparam;

            switch (lpnmhdr->code)
            {
                case PSN_SETACTIVE:
 			        hRule = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,
 				        sizeof(RULE)*MAXCODELEN);
                    hEncode = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,
                        NUMENCODEAREA*sizeof(ENCODEAREA));
                    if(!hRule || !hEncode)
                        ProcessError(ERR_OUTOFMEMORY,hdlg,ERR);		
		
                    if ( hEncode )	
			           ConvInitEncode(hEncode);
                    break;

                case PSN_APPLY:
					if(bModify) {
                       LoadString(NULL, IDS_FILEMODIFY, _szStr, sizeof(_szStr)/sizeof(TCHAR));
#ifdef UNICODE
{
		               TCHAR UniTmp[] = {0x521B, 0x5EFA, 0x8F93, 0x5165, 0x6CD5, 0x9875, 0x9762, 0x4E2D, 0x005C, 0x006E, 0x005C, 0x0027, 0x0025, 0x0077, 0x0073, 0x005C, 0x0027, 0x005C, 0x006E, 0x0025, 0x0077, 0x0073, 0x0000};		
                       wsprintf(szMbName,UniTmp,szSrcName,_szStr);
}
#else
                       wsprintf(szMbName,"�������뷨ҳ����\n\'%s\'\n%s",szSrcName,_szStr);
#endif
                       if(ErrMessage(hdlg,szMbName)) {
					        SendMessage(hdlg,WM_COMMAND,IDC_SAVE,0L);
					   }
					}
                    break;
                case PSN_RESET:
                    break;
                case PSN_QUERYCANCEL:
                    break;

                case PSN_HELP:

                    break;
                case PSN_KILLACTIVE:
                    break;
                default:
                    break;
            }

            break;

        case WM_COMMAND:

            switch (LOWORD(wparam))
            {
                case IDC_BROWSE:
					if(bModify) {
                        TCHAR errString[MAX_PATH];
                        LoadString(NULL, IDS_FILEMODIFY, _szStr, sizeof(_szStr)/sizeof(TCHAR));
                        wsprintf(errString,TEXT("\'%s\'%s"),szSrcName,_szStr);
                        if(ErrMessage(hdlg,errString))
					        SendMessage(hdlg,WM_COMMAND,IDC_SAVE,0L);
                    }
					
					SetDlgItemText(hdlg,IDC_MBNAME,szMbName);
				    {	
#ifdef UNICODE					
					static TCHAR szTitle[] = {0x6253, 0x5F00, 0x0000};
#else
					TCHAR szTitle[MAX_PATH];
					strcpy(szTitle,"��");
#endif

				    if(!TxtFileOpenDlg(hdlg,_szStr,szTitle))
				        break;
					}
					
					lstrcpy(szSrcName,_szStr);
                    bModify = FALSE;
                    memset(_szStr, 0, MAX_PATH);
 	                SetDlgItemText(hdlg,IDC_EDITBOX,_szStr);
 	                SendDlgItemMessage(hdlg,IDC_LIST,LB_RESETCONTENT,0,0L);
  				    {
					    LPTSTR lpString;
  				        if((lpString = _tcsrchr(szSrcName,TEXT('\\')))!=NULL)
					        SetDlgItemText(hdlg,IDC_SRCNAME, lpString+1);
					    lstrcpy(szMbName, szSrcName);
					    lstrcpy(_szStr, szSrcName);
					    if((lpString = _tcsrchr(szMbName,TEXT('.')))!=NULL)
					        *lpString = 0;
					    lstrcat(szMbName,TEXT(MbExt));
					}
					
					SetDlgItemText(hdlg,IDC_MBNAME,szMbName);
				    {
				        int nRetValue;
				        nRetValue = ConvGetDescript(hdlg, szSrcName,
				            &dwDesOffset, &dwDesLen, &Descript, FALSE);
				        if(nRetValue ==(-ERR_FILENOTOPEN)) {
  					        szSrcName[0]=0;
   					        lstrcpy(szMbName, szSrcName);
 					        SetDlgItemText(hdlg,IDC_SRCNAME,szSrcName);
   					        SetDlgItemText(hdlg,IDC_MBNAME,szMbName);
						    MessageBeep((UINT)-1);
				            break;
 					    }
					}

                    if(!(lpRule = GlobalLock(hRule)) )  {
                        ProcessError(ERR_GLOBALLOCK,hdlg,ERR);
						break;
				    }
 				
				    if(ConvGetRule(hdlg, szSrcName, &dwRuleOffset, &dwRuleLen,
				       lpRule, &Descript)==(-ERR_FILENOTOPEN))
					{
						szSrcName[0]=0;
						lstrcpy(szMbName,szSrcName);
 					    SetDlgItemText(hdlg,IDC_SRCNAME,szSrcName);
 					    SetDlgItemText(hdlg,IDC_MBNAME,szMbName);
						GlobalUnlock(hRule);
						MessageBeep((UINT)-1);
                        EnableWindow(GetDlgItem(hdlg,IDC_SAVE),FALSE);
						break;
					}
                    SetDlgDescript(hdlg,&Descript);
					SetDlgRuleStr(hdlg,Descript.wNumRules,lpRule);
					SetDlgItemText(hdlg,IDC_MBNAME,szMbName);

					GlobalUnlock(hRule);
					SetConvEnable(hdlg);

                    EnableWindow(GetDlgItem(hdlg,IDC_SAVE),FALSE);
					SendMessage(GetDlgItem(hdlg,ID_FILEOPEN),BM_SETSTYLE,BS_PUSHBUTTON,0L);
					SendMessage(GetDlgItem(hdlg,IDC_CONV),BM_SETSTYLE,BS_DEFPUSHBUTTON,TRUE);
					SetFocus(GetDlgItem(hdlg,IDC_CONV));
                    break;

                case IDC_MBNAME:
					GetDlgItemText(hdlg,IDC_MBNAME,szMbName,256);
					if(lstrlen(szMbName) == 0) {
                        EnableWindow(GetDlgItem(hdlg,IDC_CONV),FALSE);
					    break;
					}
					else {
					    GetDlgItemText(hdlg,IDC_IMENAME,_szStr,32);
					    if(lstrlen(_szStr) != 0 && Descript.wMaxCodes != 0)
                            EnableWindow(GetDlgItem(hdlg,IDC_CONV),TRUE);
						else
                            EnableWindow(GetDlgItem(hdlg,IDC_CONV),FALSE);
					}
					
                    break;
                case IDC_IMENAME:
					GetDlgItemText(hdlg,IDC_IMENAME,_szStr,128);
					_szStr[128]=0;
					if(lstrlen(_szStr) == 0) {
                        EnableWindow(GetDlgItem(hdlg,IDC_CONV),FALSE);
					    break;
					}
					else {
					    GetDlgItemText(hdlg,IDC_MBNAME,szMbName,256);
					    if(lstrlen(szMbName) != 0 && Descript.wMaxCodes != 0)
                            EnableWindow(GetDlgItem(hdlg,IDC_CONV),TRUE);
						else
                            EnableWindow(GetDlgItem(hdlg,IDC_CONV),FALSE);
					}
					
					if(lstrcmpi(Descript.szName,_szStr)==0)
					    break;
                    bModify = TRUE;
                    EnableWindow(GetDlgItem(hdlg,IDC_SAVE),TRUE);
                    break;

				case IDC_MAXCODES:
				{
					int nMaxCodes;

					nMaxCodes = GetDlgItemInt (hdlg,IDC_MAXCODES,NULL,FALSE);
                    if(nMaxCodes > MAXCODELEN) {
						 MessageBeep((UINT)-1);
						 SetDlgItemInt (hdlg,IDC_MAXCODES,MAXCODELEN,FALSE);
						 Descript.wMaxCodes = MAXCODELEN;
					}
                    if(Descript.wMaxCodes != nMaxCodes) {
                         bModify = TRUE;
                         EnableWindow(GetDlgItem(hdlg,IDC_SAVE),TRUE);
					}

					break;
				}
				case IDC_USEDCODE:
					
					GetDlgItemText(hdlg,IDC_USEDCODE,_szStr,sizeof(_szStr)/sizeof(TCHAR));
					
                    if(lstrlen(_szStr) == 0)
                        EnableWindow(GetDlgItem(hdlg,IDC_CONV),FALSE);
					
					if(lstrcmpi(Descript.szUsedCode,_szStr)==0)
					    break;
                    bModify = TRUE;
                    EnableWindow(GetDlgItem(hdlg,IDC_SAVE),TRUE);
                    break;

				case IDC_WILDCHAR:
					GetDlgItemText(hdlg,IDC_WILDCHAR,_szStr,sizeof(_szStr)/sizeof(TCHAR));
					if(lstrlen(_szStr)==0 ) {
					    if( Descript.cWildChar!=0) {
                             bModify = TRUE;
                             EnableWindow(GetDlgItem(hdlg,IDC_SAVE),TRUE);
						}
					    break;
					}
					if(lstrlen(_szStr)>1)  {
					    MessageBeep((UINT)-1);
						_szStr[0]=0;
						SetDlgItemText(hdlg,IDC_WILDCHAR,_szStr);
						break;
					}

					{
					    TCHAR WildChar;

					    WildChar=_szStr[0];
					    if(WildChar != Descript.cWildChar) {
                            EnableWindow(GetDlgItem(hdlg,IDC_SAVE),TRUE);
                            bModify = TRUE;
					    }
					    GetDlgItemText(hdlg,IDC_USEDCODE,_szStr,sizeof(_szStr)/sizeof(TCHAR));
					    if(_tcschr(_szStr,WildChar) != NULL) {
						    _szStr[0]=0;
						    SetDlgItemText(hdlg,IDC_WILDCHAR,_szStr);
				            MessageBeep((UINT)-1);
						    break;
					    }
					}
				    break;

				case IDC_SINGLEELEMENT:
				case IDC_MULTIELEMENT:
	                if(Descript.byMaxElement == (BYTE)(IsDlgButtonChecked(hdlg,IDC_MULTIELEMENT)?2:1))
                        break;
                    Descript.byMaxElement = 1 + Descript.byMaxElement%2;
                    bModify = TRUE;
                    EnableWindow(GetDlgItem(hdlg,IDC_SAVE),TRUE);
				    break;

				case IDC_ADD:
				{
	                FARPROC      lpProcInfo;

				    szRuleStr[0] = 0;
				    if(Descript.wNumRules >= MAXCODELEN) {
					    ProcessError(ERR_RULENUM,hdlg,WARNING);
						break;
					}

                    lpProcInfo = MakeProcInstance((FARPROC)AddRuleDlg, hInst);
                   	DialogBox(hInst,
                     		  MAKEINTRESOURCE(IDD_ADDRULE),
                    		  hdlg,
                    		  (DLGPROC)AddRuleDlg);

                    FreeProcInstance(lpProcInfo);
                    bModify = TRUE;
				    break;
				}

				case IDC_MODIFY:
				{
	                FARPROC      lpProcInfo;
				    int          nSelect;

				    nSelect = (INT)SendDlgItemMessage(hdlg,
                                                      IDC_LIST,
                                                      LB_GETCURSEL,
                                                      (WPARAM)0,
                                                      (LPARAM)0);
					if(nSelect == LB_ERR) {
					    MessageBeep((UINT)-1);
						break;
					}
				    SendDlgItemMessage(hdlg,IDC_LIST,LB_GETTEXT,nSelect,(LPARAM)szRuleStr);
                    lpProcInfo = MakeProcInstance((FARPROC)ModiRuleDlg, hInst);
                   	DialogBox(hInst,
                     		  MAKEINTRESOURCE(IDD_EDITRULE),
                    		  hdlg,
                    		  (DLGPROC)ModiRuleDlg);

                    FreeProcInstance(lpProcInfo);
                    break;

				}
				case IDC_DEL:
				{
					int nSelect;

				    nSelect = (INT)SendDlgItemMessage(hdlg,
                                                      IDC_LIST,
                                                      LB_GETCURSEL,
                                                      (WPARAM)0,
                                                      (LPARAM)0);
					if(nSelect != LB_ERR) {
				        SendDlgItemMessage(hdlg,IDC_LIST,LB_DELETESTRING,
                                           nSelect,(LPARAM)0);
                        if(!(lpRule = GlobalLock(hRule)) )  {
                             ProcessError(ERR_GLOBALLOCK,hdlg,ERR);
						     break;
						}

                        DelSelRule(Descript.wNumRules,(WORD)nSelect,lpRule);
						Descript.wNumRules--;
						GlobalUnlock(hRule);
                        bModify = TRUE;
                        EnableWindow(GetDlgItem(hdlg,IDC_SAVE),TRUE);

					    SetFocus(GetDlgItem(hdlg,IDC_LIST));
					    SendDlgItemMessage(hdlg,IDC_LIST,LB_SETCURSEL,nSelect,0L);
   					    if((nSelect=(INT)SendDlgItemMessage(hdlg,IDC_LIST,
                                         LB_GETCURSEL,0,0L))==LB_ERR) {

					        nSelect=(INT)SendDlgItemMessage(hdlg,
                                                            IDC_LIST,
                                                            LB_GETCOUNT,
                                                            (WPARAM)0,
                                                            (LPARAM)0);
					        SendDlgItemMessage(hdlg,
                                               IDC_LIST,
                                               LB_SETCURSEL,
                                               (WPARAM)(nSelect-1),
                                               (LPARAM)0);
                        }
				    }
					else
					    MessageBeep((UINT)-1);
					
				    break;
				}

				case IDC_ADDRULE:
                    if(!(lpRule = GlobalLock(hRule)) )  {
                        ProcessError(ERR_GLOBALLOCK,hdlg,ERR);
				        szRuleStr[0] = 0;
						break;
				    }

                    if(RuleParse(hdlg,szRuleStr,0,lpRule,Descript.wMaxCodes)) {
						int nCnt;

	                    nCnt =(WORD)SendDlgItemMessage(hdlg,IDC_LIST,LB_GETCOUNT,0,0L);
                        if(nCnt != 0)
	                    {
	                        for(i=0; i<nCnt; i++) {
                                SendDlgItemMessage(hdlg,IDC_LIST,LB_GETTEXT,i,(LPARAM)_szStr);
#ifdef UNICODE
						        if(wcsncmp(szRuleStr,_szStr,3) == 0)
#else
								if(strncmp(szRuleStr,_szStr,3) == 0)
#endif //UNICODE
						  	    {
					                 GlobalUnlock(hRule);
									
      				                 ProcessError(ERR_DUPRULE,hdlg,ERR);
									 MessageBeep((UINT)-1);
									 MessageBeep((UINT)-1);
									 _szStr[0] = 0;
									 lstrcpy((LPTSTR)lparam,(LPTSTR)_szStr);
									 return FALSE;
								}
							}
						}
						SendDlgItemMessage(hdlg,IDC_LIST,LB_ADDSTRING,0,(LPARAM)szRuleStr);
                        bModify = TRUE;
                        EnableWindow(GetDlgItem(hdlg,IDC_SAVE),TRUE);
					    Descript.wNumRules++;
					}
					else
					{
					    MessageBeep((UINT)-1);
						_szStr[0] = 0;
						lstrcpy((LPTSTR)lparam,(LPTSTR)_szStr);
					}
					GlobalUnlock(hRule);

				    break;

				case IDC_CHGDATA:
                    if(!(lpRule = GlobalLock(hRule)) )  {
                        ProcessError(ERR_GLOBALLOCK,hdlg,ERR);
				        _szStr[0] = 0;
						lstrcpy((LPTSTR)lparam,(LPTSTR)_szStr);
						break;
				    }

					{
				    int nSelect;

				    nSelect = (INT)SendDlgItemMessage(hdlg,IDC_LIST,LB_GETCURSEL,0,0L);
                    if(RuleParse(hdlg,szRuleStr,nSelect,lpRule,Descript.wMaxCodes)) {
//					    Descript.wNumRules++;
						int nCnt;

	                    nCnt =(WORD)SendDlgItemMessage(hdlg,IDC_LIST,LB_GETCOUNT,0,0L);
                        if(nCnt != 0)
	                    {
	                        for(i=0; i<nCnt; i++) {
                                if(i == nSelect)
								     continue;
                                SendDlgItemMessage(hdlg,IDC_LIST,LB_GETTEXT,i,(LPARAM)_szStr);
#ifdef UNICODE
						        if(wcsncmp(szRuleStr,_szStr,3) == 0)
#else
								if(strncmp(szRuleStr,_szStr,3) == 0)
#endif
						  	    {
					                 GlobalUnlock(hRule);
									 MessageBeep((UINT)-1);
				                     _szStr[0] = 0;
						             lstrcpy((LPTSTR)lparam,_szStr);
									 return FALSE;
								}
							}
						}
						SendDlgItemMessage(hdlg,IDC_LIST,LB_DELETESTRING,nSelect,0L);
						SendDlgItemMessage(hdlg,IDC_LIST,LB_ADDSTRING,0,(LPARAM)szRuleStr);
                        bModify = TRUE;
                        EnableWindow(GetDlgItem(hdlg,IDC_SAVE),TRUE);
						
					}
					else
				    {
				        _szStr[0] = 0;
						lstrcpy((LPTSTR)lparam,_szStr);
					    MessageBeep((UINT)-1);
					}
					GlobalUnlock(hRule);

				    break;
				    }
				case IDC_GETMBFILE:
					GetDlgItemText(hdlg,IDC_MBNAME,_szStr,sizeof(_szStr)/sizeof(TCHAR));
					lstrcpy((LPTSTR)lparam,_szStr);
					break;

				case IDC_GETSRCFILE:
					lstrcpy((LPTSTR)lparam,szSrcName);
					break;

				case IDC_CONV:
				{
	                FARPROC      lpProcInfo;

                    if(bModify)
						 SendMessage(hdlg,WM_COMMAND,IDC_SAVE,0L);
                    lpProcInfo = MakeProcInstance((FARPROC)InfoDlg, hInst);
					pfnmsg=(PFNMSG)ConvProc;
					bEndProp=FALSE;
                   	DialogBox(hInst,
                     		  MAKEINTRESOURCE(IDD_INFO),
                    		  hdlg,
                    		  (DLGPROC)lpProcInfo);
                    FreeProcInstance(lpProcInfo);
				    break;
				}
				case IDC_CRTIME:
				{
	                FARPROC      lpProcInfo;
                    int 		 nRetValue;
					GetDlgItemText(hdlg, IDC_MBNAME, _szStr, sizeof(_szStr)/sizeof(TCHAR));
                    nRetValue = ReadDescript(_szStr,&Descript,FILE_SHARE_READ);
                    if(nRetValue == -1){
                        TCHAR errString[MAX_PATH];
                        LoadString(NULL, IDS_FILEOPEN, errString, sizeof(errString)/sizeof(TCHAR));
						lstrcat(_szStr, errString);
                        FatalMessage(hdlg,_szStr);
						break;
					}
					else if(!nRetValue)
					{
					    ProcessError(ERR_READMAININDEX,hdlg,WARNING);
   						break;
				    }
                    lpProcInfo = MakeProcInstance((FARPROC)CrtImeDlg, hInst);
                   	DialogBox(hInst,
                     		  MAKEINTRESOURCE(IDD_CRTIME),
                    		  hdlg,
                    		  (DLGPROC)lpProcInfo);
                    FreeProcInstance(lpProcInfo);
				    break;
				}
				
				case IDC_SAVE:
                    if(!(lpRule = GlobalLock(hRule)) )  {
                        ProcessError(ERR_GLOBALLOCK,hdlg,ERR);
				        _szStr[0] = 0;
						lstrcpy((LPTSTR)lparam,_szStr);
						break;
				    }
					GetDlgDescript(hdlg,&Descript);
					if(!CheckCodeCollection(hdlg,Descript.szUsedCode))
					    break;
					GetDlgRule(hdlg,lpRule,&(Descript.wNumRules),Descript.wMaxCodes);
				    if(!ConvSaveDescript(szSrcName,&Descript, dwDesOffset,dwDesLen))
				        break;
                    //**** modify 95.10.11
                    ConvGetDescript(hdlg,szSrcName,&dwDesOffset,&dwDesLen,&Descript,TRUE);
					//**** end modify
					
                    if(!(lpRule = GlobalLock(hRule)) )  {
                        ProcessError(ERR_GLOBALLOCK,hdlg,ERR);
					    break;
					}
                    if(!ConvSaveRule(hdlg,szSrcName, dwDesLen,dwRuleLen,
                       lpRule, Descript.wNumRules))
                        break;
 				    ConvGetRule(hdlg, szSrcName, &dwRuleOffset, &dwRuleLen,
				        lpRule, &Descript);
					GlobalUnlock(hRule);
                    bModify = FALSE;
                    EnableWindow(GetDlgItem(hdlg,IDC_CONV),TRUE);
                    EnableWindow(GetDlgItem(hdlg,IDC_SAVE),FALSE);
					SendMessage(GetDlgItem(hdlg,IDC_SAVE),BM_SETSTYLE,BS_PUSHBUTTON,0L);
					SendMessage(GetDlgItem(hdlg,IDC_CONV),BM_SETSTYLE,BS_DEFPUSHBUTTON,TRUE);
					SetFocus(GetDlgItem(hdlg,IDC_CONV));
					break;

				case IDC_HLP:
				{
                    TCHAR szHlpName[MAX_PATH];

                    szHlpName[0] = 0;
                    GetWindowsDirectory((LPTSTR)szHlpName, MAX_PATH);
                    lstrcat(szHlpName, TEXT("\\HELP\\IMEGEN.CHM"));
                    HtmlHelp(hdlg,szHlpName,HH_DISPLAY_TOPIC,0L);
				}
					break;
                default:
                    break;
            }

            break;

        default:
            break;
    }

    return FALSE;
}

VOID ConvProc(LPVOID hWnd)
{
	static TCHAR file1[MAX_PATH]=TEXT("");
	static TCHAR file2[MAX_PATH]=TEXT("");
  	SendMessage(GetParent(hDlgless),WM_COMMAND,IDC_GETSRCFILE,(LPARAM)file1);
  	SendMessage(GetParent(hDlgless),WM_COMMAND,IDC_GETMBFILE,(LPARAM)file2);
  	if(ConvConv(hDlgless,file1,file2))
	   bEndProp=TRUE;
	SendMessage(hDlgless,WM_CLOSE,0,0L);
}

void SetDlgDescript(HWND hDlg,LPDESCRIPTION lpDescript)
{
	short nElement;
	TCHAR _szStr[20];

	nElement =(lpDescript->byMaxElement==1)?IDC_SINGLEELEMENT:IDC_MULTIELEMENT;
	SetDlgItemText(hDlg,IDC_IMENAME,lpDescript->szName);
	SetDlgItemText(hDlg,IDC_USEDCODE,lpDescript->szUsedCode);
	_szStr[0]=lpDescript->cWildChar;
	_szStr[1]=0;
	SetDlgItemText(hDlg,IDC_WILDCHAR,_szStr);
	SetDlgItemInt (hDlg,IDC_MAXCODES,lpDescript->wMaxCodes,FALSE);
	CheckRadioButton(hDlg,IDC_SINGLEELEMENT,IDC_MULTIELEMENT,nElement);
}

void GetDlgDescript(HWND hDlg,LPDESCRIPTION lpDescript)
{

	BOOL bTrans;
	TCHAR _szStr[20];

	GetDlgItemText(hDlg,IDC_IMENAME,lpDescript->szName,NAMESIZE);
	GetDlgItemText(hDlg,IDC_USEDCODE,lpDescript->szUsedCode,MAXUSEDCODES);
	GetDlgItemText(hDlg,IDC_WILDCHAR,_szStr,sizeof(_szStr)/sizeof(TCHAR));
	DelSpace(_szStr);
	if(_szStr[0]==0) _szStr[0]=TEXT('?');
	lpDescript->cWildChar=_szStr[0];
	lpDescript->wMaxCodes=(WORD)GetDlgItemInt (hDlg,IDC_MAXCODES,&bTrans,FALSE);
	lpDescript->wNumCodes=(WORD)lstrlen(lpDescript->szUsedCode);
	lpDescript->byMaxElement=(BYTE)(IsDlgButtonChecked(hDlg,IDC_MULTIELEMENT)?2:1);
	lpDescript->wNumRules=(WORD)SendDlgItemMessage(hDlg,IDC_LIST,LB_GETCOUNT,0,0L);
}


void SetDlgRuleStr(HWND hDlg,WORD NumRules,LPRULE lpRule)
{
    WORD  i;
	TCHAR _szStr[MAX_PATH];

	SendDlgItemMessage(hDlg,IDC_LIST,LB_RESETCONTENT,0,0L);
    if(NumRules==0)  return;
	for(i=0; i<NumRules; i++) {
        RuleToText(&lpRule[i], _szStr);
		_szStr[lstrlen(_szStr)-2]=0;
		SendDlgItemMessage(hDlg,IDC_LIST,LB_ADDSTRING,0,(LPARAM)_szStr);
	}
}

void GetDlgRule(HWND hdlg,LPRULE lpRule,LPWORD fwNumRules,WORD MaxCodes)
{
    int  i;
	TCHAR _szStr[128];
	WORD NumRules = 0;

	lpRule = GlobalLock(hRule);
	*fwNumRules =(WORD)SendDlgItemMessage(hdlg,IDC_LIST,LB_GETCOUNT,0,0L);
    if((*fwNumRules) == 0)
    {
        GlobalUnlock(hRule);
        return;
	}
	for(i=0;i<(int)*fwNumRules;i++) {
        SendDlgItemMessage(hdlg,IDC_LIST,LB_GETTEXT,i,(LPARAM)_szStr);
        if(RuleParse(hdlg,_szStr,i,lpRule,MaxCodes))
		    NumRules++;
	}

    GlobalUnlock(hRule);
	*fwNumRules = NumRules;
}
			
void DelSelRule(WORD wNumRules,WORD wSelect,LPRULE lpRule)
{
    int i;
	if(wSelect>=MAXCODELEN) return;
	for(i=wSelect;i<wNumRules;i++)
	    lpRule[i]=lpRule[i+1];
}

void SetConvDisable(HWND hDlg)
{
	WORD wID;

	for(wID = IDC_MBNAME ;wID <= IDC_CONV ;wID++)
        EnableWindow(GetDlgItem(hDlg,wID),FALSE);

    EnableWindow(GetDlgItem(hDlg,IDC_CRTIME),FALSE);
	for(wID = IDC_STATIC1 ;wID <= IDC_STATIC5 ;wID++)
        EnableWindow(GetDlgItem(hDlg,wID),FALSE);
}

void SetConvEnable(HWND hDlg)
{
    WORD wID;

	for(wID = IDC_MBNAME ;wID <= IDC_CONV ;wID++)
        EnableWindow(GetDlgItem(hDlg,wID),TRUE);
    EnableWindow(GetDlgItem(hDlg,IDC_CRTIME),TRUE);
	for(wID = IDC_STATIC1 ;wID <= IDC_STATIC5 ;wID++)
        EnableWindow(GetDlgItem(hDlg,wID),TRUE);

}

INT_PTR  CALLBACK  ConvEditProc(HWND   hWnd,
 				 			   UINT   wMsgID,
							   WPARAM wParam,
							   LPARAM lParam)
{
    switch(wMsgID) {
		case WM_LBUTTONDBLCLK:
			SendMessage(GetParent(hWnd),WM_COMMAND,IDC_MODIFY,0L);
		    break;

	    case WM_KEYDOWN:

		    switch(wParam) {
				case VK_DELETE:
				    SendMessage(GetParent(hWnd),WM_COMMAND,IDC_DEL,0L);
					break;
			}

		default:
		    return CallWindowProc((WNDPROC)lpConvProc,hWnd,wMsgID,wParam,lParam);
	}
	return FALSE;
}

void InstallConvSubClass(HWND hWnd)
{
    lpConvProc = (FARPROC)SetWindowLongPtr(hWnd,GWLP_WNDPROC,(LONG_PTR)ConvEditProc);
}


INT_PTR  CALLBACK AddRuleDlg(
        HWND   hDlg,
        UINT   message,
        WPARAM wParam,
        LPARAM lParam)
{
	LPRULE  lpRule;

    switch (message) {
        case WM_INITDIALOG:
			lpRule=GlobalLock(hRule);
			szRuleStr[0] = 0;
			SetDlgItemText(hDlg,IDC_EDITRULE,szRuleStr);
            return (TRUE);

        case WM_COMMAND:
            switch(LOWORD(wParam)) {

                case IDOK:
				    GetDlgItemText(hDlg,IDC_EDITRULE,szRuleStr,sizeof(szRuleStr)/sizeof(TCHAR));
					SendMessage(GetParent(hDlg),WM_COMMAND,IDC_ADDRULE,(LPARAM)szRuleStr);
					if(*szRuleStr == 0)
					{
					 	SetFocus(GetDlgItem(hDlg,IDC_EDITRULE));
					    return (TRUE);
					}
					GlobalUnlock(hRule);
                    EndDialog(hDlg, TRUE);
                    return (TRUE);

				case IDCANCEL:
				case WM_CLOSE:
					GlobalUnlock(hRule);
                    EndDialog(hDlg, TRUE);
					break;

				default:
				    break;
            }
            break;
    }
    return (FALSE);
        UNREFERENCED_PARAMETER(lParam);
}

INT_PTR  CALLBACK ModiRuleDlg(
        HWND   hDlg,
        UINT   message,
        WPARAM wParam,
        LPARAM lParam)
{
	LPRULE  lpRule;

    switch (message) {
        case WM_INITDIALOG:
			lpRule=GlobalLock(hRule);
			SetDlgItemText(hDlg,IDC_EDITRULE,szRuleStr);
            return (TRUE);

        case WM_COMMAND:
            switch(LOWORD(wParam)) {

                case IDOK:
				    GetDlgItemText(hDlg,IDC_EDITRULE,szRuleStr,sizeof(szRuleStr)/sizeof(TCHAR));
					SendMessage(GetParent(hDlg),WM_COMMAND,IDC_CHGDATA,(LPARAM)szRuleStr);
					if(*szRuleStr == 0)
					{
					 	SetFocus(GetDlgItem(hDlg,IDC_EDITRULE));
					    return (TRUE);
					}
					GlobalUnlock(hRule);
                    EndDialog(hDlg, TRUE);
                    return (TRUE);

				case IDCANCEL:
				case WM_CLOSE:
					GlobalUnlock(hRule);
                    EndDialog(hDlg, TRUE);
					break;

				default:
				    break;
            }
            break;
    }
    return (FALSE);
        UNREFERENCED_PARAMETER(lParam);
}

//**********************************************************
//SetupIme(ImeFileName, ImeLayoutName);
//**********************************************************
/*
BOOL SetupIme(
LPTSTR ImeFileName,	 //ime file name with full path
LPTSTR MBFileName,
LPTSTR ImeLayoutName)//ime layout name(in chinese)
{
	HKEY  hKeyCurrVersion, hKey, hNewKey;
	long retCode,i;
	UCHAR Buf[256], lpszName[256],LayoutHandleName[10];
    DWORD   dwDisposition;

	memset(Buf,0,256);
	memset(lpszName,0,256);
	memset(LayoutHandleName, 0, 10);

	//create registry in keyboard layout
    retCode = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                            REGSTR_PATH_CURRENT_CONTROL_SET,
                            (DWORD)0,
                            KEY_ENUMERATE_SUB_KEYS |
                            KEY_EXECUTE |
                            KEY_QUERY_VALUE,
                            &hKeyCurrVersion);
     if (retCode) {
	    wsprintf (Buf, "����: RegOpenKeyEx = %d", retCode);
	    MessageBox (NULL, Buf, "����", MB_OK | MB_ICONINFORMATION);
	    return FALSE;
    }

    retCode = RegOpenKeyEx (hKeyCurrVersion,
                           "Keyboard Layouts",
                           0,
                           KEY_ENUMERATE_SUB_KEYS |
                           KEY_EXECUTE |
                           KEY_QUERY_VALUE,
                           &hKey);
    if (retCode) {
	    wsprintf (Buf, "����: RegOpenKeyEx = %d", retCode);
	    MessageBox (NULL, Buf, "����", MB_OK | MB_ICONINFORMATION);
	    return FALSE;
    }

	for(i=0;;i++){
		retCode = RegEnumKey(hKey,	// handle of key to query
    					 i,	// index of subkey to query
    					 lpszName,	// address of buffer for subkey name
    					 256); 	// size of subkey buffer
		if(retCode)
			break;
		else{
			if(strcmp(Buf, lpszName)<0)
				strcpy(Buf, lpszName);
		}
	}
	if(Buf[0]=='\0')
		return FALSE;
	if(Buf[3]=='f' || Buf[3]=='F'){
		Buf[3]== '0';
		Buf[2]++;
	}else if(Buf[3]=='9')
		Buf[3]='A';
	else
		Buf[3]++;
	strcpy(LayoutHandleName,Buf);

    retCode = RegCreateKeyEx (hKey,
                           	  LayoutHandleName,
                              0,
							  0,
							  REG_OPTION_NON_VOLATILE,
							  KEY_ALL_ACCESS,
							  NULL,
                              &hNewKey,
                              &dwDisposition);

    if (retCode) {
	    wsprintf (Buf, "����: RegOpenKeyEx = %d", retCode);
	    MessageBox (NULL, Buf, "����", MB_OK | MB_ICONINFORMATION);
	    return FALSE;
    }

	GetFileTitle(ImeFileName, Buf, MAX_PATH);
    retCode = RegSetValueEx (hNewKey,
    			   "IME file",
		           (DWORD)NULL,
		           REG_SZ,
		           (LPBYTE)Buf,
		           strlen(Buf));
    if (retCode) {
	    wsprintf (Buf, "����: RegSetValueEx = %d", retCode);
	    MessageBox (NULL, Buf, "����", MB_OK | MB_ICONINFORMATION);
	    return FALSE;
    }

	strcpy(Buf, "kbdus.kbd");
    retCode = RegSetValueEx (hNewKey,
    			   "layout file",
		           (DWORD)NULL,
		           REG_SZ,
		           (LPBYTE)Buf,
		           strlen(Buf));
    if (retCode) {
	    wsprintf (Buf, "����: RegSetValueEx = %d", retCode);
	    MessageBox (NULL, Buf, "����", MB_OK | MB_ICONINFORMATION);
	    return FALSE;
    }

	strcpy(Buf, ImeLayoutName);
    retCode = RegSetValueEx (hNewKey,
    			   "layout text",
		           (DWORD)NULL,
		           REG_SZ,
		           (LPBYTE)Buf,
		           strlen(Buf));
    if (retCode) {
	    wsprintf (Buf, "����: RegSetValueEx = %d", retCode);
	    MessageBox (NULL, Buf, "����", MB_OK | MB_ICONINFORMATION);
	    return FALSE;
    }

	RegCloseKey(hNewKey);
	RegCloseKey(hKey);
	RegCloseKey(hKeyCurrVersion);


	//create registry in preload
    retCode = RegOpenKeyEx (HKEY_CURRENT_USER,
                            "Keyboard Layout\\Preload",
                            0,
                            KEY_ENUMERATE_SUB_KEYS |
                            KEY_EXECUTE |
                            KEY_QUERY_VALUE,
                            &hKeyCurrVersion);
     if (retCode) {
	    wsprintf (Buf, "����: RegOpenKeyEx = %d", retCode);
	    MessageBox (NULL, Buf, "����", MB_OK | MB_ICONINFORMATION);
	    return FALSE;
    }

	memset(Buf,0,256);
	memset(lpszName,0,256);
	for(i=0;;i++){
		retCode = RegEnumKey(hKeyCurrVersion,	// handle of key to query
    					 i,	// index of subkey to query
    					 lpszName,	// address of buffer for subkey name
    					 256); 	// size of subkey buffer
		if(retCode)
			break;
		else{
			if(strcmp(Buf, lpszName)<0)
				strcpy(Buf, lpszName);
		}
	}
	if(Buf[0]=='\0')
		return FALSE;
	i=_ttoi(Buf);
	i++;
	_itoa(i,Buf,10);

    retCode = RegCreateKeyEx (hKeyCurrVersion,
                           	  Buf,
                              0,
							  0,
							  REG_OPTION_NON_VOLATILE,
							  KEY_ALL_ACCESS,
							  NULL,
                              &hNewKey,
                              &dwDisposition);

    if (retCode) {
	    wsprintf (Buf, "����: RegOpenKeyEx = %d", retCode);
	    MessageBox (NULL, Buf, "����", MB_OK | MB_ICONINFORMATION);
	    return FALSE;
    }

    retCode = RegSetValueEx (hNewKey,
    			   NULL,
		           (DWORD)NULL,
		           REG_SZ,
		           (LPBYTE)LayoutHandleName,
		           strlen(LayoutHandleName));
    if (retCode) {
	    wsprintf (Buf, "����: RegSetValueEx = %d", retCode);
	    MessageBox (NULL, Buf, "����", MB_OK | MB_ICONINFORMATION);
	    return FALSE;
    }

	RegCloseKey(hNewKey);
	RegCloseKey(hKey);
	RegCloseKey(hKeyCurrVersion);

	//copy files
	{
	UCHAR	DesFilePath[MAX_PATH];

	GetSystemDirectory(DesFilePath,MAX_PATH);
	strcat(DesFilePath,"\\");

	GetFileTitle(ImeFileName, Buf, MAX_PATH);
	strcat(DesFilePath,Buf);
	CopyFile(ImeFileName, DesFilePath, FALSE);

	GetSystemDirectory(DesFilePath,MAX_PATH);
	strcat(DesFilePath,"\\");

	GetFileTitle(MBFileName, Buf, MAX_PATH);
	strcat(DesFilePath,Buf);
	CopyFile(MBFileName, DesFilePath, FALSE);

	}
	return TRUE;
}
*/

//Hack for #62554 10/29/96
HWND HwndCrtImeDlg = NULL;

INT_PTR  CALLBACK CrtImeDlg(
        HWND   hDlg,
        UINT   message,
        WPARAM wParam,
        LPARAM lParam)
{
    static TCHAR _szStr[MAX_PATH];
    static TCHAR ImeTplName[MAX_PATH];
    static TCHAR mbName[MAX_PATH];
	static IMERES ImeRes;
	LPTSTR       lpString;

	HwndCrtImeDlg = hDlg;
    switch (message) {
        case WM_INITDIALOG:
			SendMessage(GetParent(hDlg),WM_COMMAND,IDC_GETMBFILE,(LPARAM)mbName);
		    SendDlgItemMessage(hDlg,IDC_VERSION,EM_LIMITTEXT,8,0L);
		    SendDlgItemMessage(hDlg,IDC_GROUP,EM_LIMITTEXT,32,0L);
		    SetDlgItemText(hDlg, IDC_VERSION,TEXT(DefVer));
		    SetDlgItemText(hDlg, IDC_GROUP, TEXT(DefOrgName));
			ImeRes.Value = 0x00af;
			SetValue(hDlg,ImeRes.Value);
			SendMessage(hDlg,WM_COMMAND, IDC_DEF, 0L);
			CheckRadioButton(hDlg,IDC_DEF,IDC_USERDEF,IDC_DEF);
            break;

        case WM_COMMAND:
            switch(LOWORD(wParam)) {

                case IDOK:
					{
					DESCRIPTION Descript;
                    TCHAR       tmpBuf[MAX_PATH];

					if(!GetImeRes(hDlg,&ImeRes))
					    return TRUE;
                    ReadDescript(mbName,&Descript,FILE_SHARE_READ);
					lstrcpy(_szStr, Descript.szName);
					lstrcat(_szStr, szVer);
					lstrcat(_szStr, ImeRes.Version);
					lstrcpy(ImeRes.Version, _szStr);
					GetSystemDirectory(ImeTplName,sizeof(ImeTplName)/sizeof(TCHAR));
                    lstrcpy(_szStr, ImeTplName);
					lstrcat(ImeTplName, TEXT(TplName));

                    lstrcat(_szStr, TEXT("\\"));
                    GetFileTitle(mbName, tmpBuf, MAX_PATH);
                    lstrcat(_szStr, tmpBuf);

					if((lpString=_tcsrchr(_szStr,TEXT('.')))!=NULL)
					    *lpString=0;
					lstrcat(_szStr,TEXT(ImeExt));

					
					DispInfo(GetWindow(hDlg, GW_OWNER),IDS_UPRES);
					

					CopyFile(ImeTplName, _szStr, FALSE);
					

                    if(ImeUpdateRes(_szStr, ImeRes.BmpName, ImeRes.IcoName,ImeRes.Version,
					    ImeRes.Depart, ImeRes.Value)==TRUE){
						TCHAR szDBCS[256];
						LoadString(NULL,IDS_SETUPIME,szDBCS, sizeof(szDBCS)/sizeof(TCHAR));
      					if(ErrMessage(hDlg, szDBCS))
	  					{
	  					HKL hKL;
	  					TCHAR DesFilePath[MAX_PATH],Buf[MAX_PATH];
	  					
							hKL = ImmInstallIME(_szStr,Descript.szName);
							//copy .MB & .HLP to system directory.
							if(hKL){
								GetSystemDirectory(DesFilePath,MAX_PATH);
								lstrcat(DesFilePath,TEXT("\\"));
								GetFileTitle(mbName, Buf, MAX_PATH);
								lstrcat(DesFilePath,Buf);
								CopyFile(mbName, DesFilePath, FALSE);
								if(ImeRes.HlpFile[0]){
									lstrcpy(_szStr,DesFilePath);
									if((lpString=_tcsrchr(_szStr,TEXT('.')))!=NULL)
					    			*lpString=0;
									lstrcat(_szStr,TEXT(HlpExt));
									CopyFile( ImeRes.HlpFile, _szStr, FALSE);

									if((lpString=_tcsrchr(ImeRes.HlpFile,TEXT('.')))!=NULL)
					    			*lpString=0;
									lstrcat(ImeRes.HlpFile,TEXT(".CNT"));
									lstrcpy(_szStr,DesFilePath);
									if((lpString=_tcsrchr(_szStr,TEXT('\\')))!=NULL)
					    			*(lpString+1)=0;
									GetFileTitle(ImeRes.HlpFile, Buf, MAX_PATH);
									lstrcat(_szStr,Buf);
									CopyFile( ImeRes.HlpFile, _szStr, FALSE);

									if((lpString=_tcsrchr(ImeRes.HlpFile,TEXT('.')))!=NULL)
					    			*(lpString+1)=0;
									lstrcat(ImeRes.HlpFile,TEXT(".GID"));
									lstrcpy(_szStr,DesFilePath);
									if((lpString=_tcsrchr(_szStr,TEXT('\\')))!=NULL)
					    			*lpString=0;
									GetFileTitle(ImeRes.HlpFile, Buf, MAX_PATH);
									lstrcat(_szStr,Buf);
									CopyFile( ImeRes.HlpFile, _szStr, FALSE);

								}
							}else{
								LoadString(NULL,IDS_ERR_INSTALLIME,_szStr, sizeof(_szStr)/sizeof(TCHAR));
								WarnMessage(hDlg,_szStr);
							}
						}
					}

                    EndDialog(hDlg, TRUE);
                    return (TRUE);
					}
				
				case IDC_BROWSE:
				    if(!RcFileOpenDlg(hDlg, _szStr,Title))
				        break;
					if(_tcsstr(_tcsupr(_szStr),TEXT(BmpExt)) != NULL)
					    SetDlgItemText(hDlg, IDC_BMP, _szStr);
#ifdef UNICODE
					else if(_tcsstr(_wcsupr(_szStr),TEXT(IconExt)) != NULL)
#else
					else if(_tcsstr(_strupr(_szStr),TEXT(IconExt)) != NULL)
#endif

					    SetDlgItemText(hDlg, IDC_ICO, _szStr);
#ifdef UNICODE
					else if(_tcsstr(_wcsupr(_szStr),TEXT(HlpExt)) != NULL)
#else
					else if(_tcsstr(_strupr(_szStr),TEXT(HlpExt)) != NULL)
#endif
					    SetDlgItemText(hDlg, IDC_HLPFILE, _szStr);
					break;
				case IDC_DEF:
				    EnableWindow(GetDlgItem(hDlg,IDC_BMP), FALSE);
				    EnableWindow(GetDlgItem(hDlg,IDC_ICO), FALSE);
				    EnableWindow(GetDlgItem(hDlg,IDC_HLPFILE), FALSE);
				    EnableWindow(GetDlgItem(hDlg,IDC_BROWSE), FALSE);
					GetDlgItemText(hDlg, IDC_ICO, ImeRes.IcoName,sizeof(ImeRes.IcoName)/sizeof(TCHAR));
					GetDlgItemText(hDlg, IDC_BMP, ImeRes.BmpName,sizeof(ImeRes.BmpName)/sizeof(TCHAR) );
					GetDlgItemText(hDlg, IDC_BMP, ImeRes.HlpFile,sizeof(ImeRes.HlpFile)/sizeof(TCHAR));
					_szStr[0] = 0;
					SetDlgItemText(hDlg, IDC_ICO, _szStr);
					SetDlgItemText(hDlg, IDC_BMP, _szStr);
					SetDlgItemText(hDlg, IDC_HLPFILE, _szStr);
					break;


				case IDC_USERDEF:
				    EnableWindow(GetDlgItem(hDlg,IDC_BMP), TRUE);
				    EnableWindow(GetDlgItem(hDlg,IDC_ICO), TRUE);
				    EnableWindow(GetDlgItem(hDlg,IDC_HLPFILE), TRUE);
				    EnableWindow(GetDlgItem(hDlg,IDC_BROWSE), TRUE);
					SetDlgItemText(hDlg, IDC_ICO, ImeRes.IcoName);
					SetDlgItemText(hDlg, IDC_BMP, ImeRes.BmpName);
					SetDlgItemText(hDlg, IDC_HLPFILE, ImeRes.HlpFile);
					break;
				
				case IDC_ZJTS:
					if(IsDlgButtonChecked(hDlg,IDC_ZJTS))
						EnableWindow(GetDlgItem(hDlg,IDC_WMTS),TRUE);
					else {
					    CheckDlgButton(hDlg,IDC_WMTS,0);
						EnableWindow(GetDlgItem(hDlg,IDC_WMTS),FALSE);
					}
				    break;

				case IDCANCEL:
				case WM_CLOSE:
                    EndDialog(hDlg, TRUE);
					return (TRUE);

				default:
				    break;
            }
            break;
    }
    return (FALSE);
        UNREFERENCED_PARAMETER(lParam);
}

void SetValue(HWND hDlg, WORD Value)
{
	
	CheckDlgButton(hDlg,IDC_CYLX,Value&0x0001);
	CheckDlgButton(hDlg,IDC_CYSR,Value&0x0002);
	CheckDlgButton(hDlg,IDC_ZJTS,Value&0x0004);
	if((Value&0x0004)==0) {
	    EnableWindow(GetDlgItem(hDlg,IDC_WMTS),FALSE);
		Value &= ~ 0x0008;
	}
	CheckDlgButton(hDlg,IDC_WMTS,Value&0x0008);
	CheckDlgButton(hDlg,IDC_GBGS,Value&0x0020);
	CheckDlgButton(hDlg,IDC_SPACE,Value&0x0040);
	CheckDlgButton(hDlg,IDC_ENTER,Value&0x0080);
}

void GetValue(HWND hDlg,LPWORD Value)
{
	*Value = 0;
	*Value |= IsDlgButtonChecked(hDlg,IDC_CYLX);
	*Value |= IsDlgButtonChecked(hDlg,IDC_CYSR) << 1;
	*Value |= IsDlgButtonChecked(hDlg,IDC_ZJTS) << 2;
	*Value |= IsDlgButtonChecked(hDlg,IDC_WMTS) << 3;
	*Value |= IsDlgButtonChecked(hDlg,IDC_GBGS) << 5;
	*Value |= IsDlgButtonChecked(hDlg,IDC_SPACE) << 6;
	*Value |= IsDlgButtonChecked(hDlg,IDC_ENTER) << 7;
}

int  GetImeRes(HWND hDlg,LPIMERES lpImeRes)
{
	GetDlgItemText(hDlg, IDC_VERSION, lpImeRes->Version, sizeof(lpImeRes->Version)/sizeof(TCHAR));
	if(lstrlen(lpImeRes->Version) == 0)
	{
	    ProcessError(ERR_VERSION, hDlg, WARNING);
	    return FALSE;
	}
	GetDlgItemText(hDlg, IDC_GROUP,   lpImeRes->Depart, sizeof(lpImeRes->Depart)/sizeof(TCHAR));
	if(lstrlen(lpImeRes->Depart) == 0)
	{
	    ProcessError(ERR_GROUP, hDlg, WARNING);
	    return FALSE;
	}
	GetDlgItemText(hDlg, IDC_BMP,     lpImeRes->BmpName, sizeof(lpImeRes->BmpName)/sizeof(TCHAR));
	GetDlgItemText(hDlg, IDC_ICO,     lpImeRes->IcoName, sizeof(lpImeRes->IcoName)/sizeof(TCHAR));
	GetValue(hDlg, &(lpImeRes->Value));
	return TRUE;
}

