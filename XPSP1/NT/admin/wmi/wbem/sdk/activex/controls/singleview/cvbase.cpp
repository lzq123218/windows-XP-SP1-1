// Copyright (c) 1997-2001 Microsoft Corporation, All Rights Reserved
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "precomp.h"
#include "cvbase.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomViewBase

IMPLEMENT_DYNCREATE(CCustomViewBase, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CCustomViewBase properties

CString CCustomViewBase::GetNameSpace()
{
	CString result;
	GetProperty(0x1, VT_BSTR, (void*)&result);
	return result;
}

void CCustomViewBase::SetNameSpace(LPCTSTR propVal)
{
	SetProperty(0x1, VT_BSTR, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// CCustomViewBase operations

long CCustomViewBase::QueryNeedsSave()
{
	long result;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long CCustomViewBase::AddContextRef(long lCtxtHandle)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		lCtxtHandle);
	return result;
}

long CCustomViewBase::GetContext(long* plCtxthandle)
{
	long result;
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		plCtxthandle);
	return result;
}

long CCustomViewBase::GetEditMode()
{
	long result;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

void CCustomViewBase::ExternInstanceCreated(LPCTSTR szObjectPath)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 szObjectPath);
}

void CCustomViewBase::ExternInstanceDeleted(LPCTSTR szObjectPath)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 szObjectPath);
}

long CCustomViewBase::RefreshView()
{
	long result;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long CCustomViewBase::ReleaseContext(long lCtxtHandle)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		lCtxtHandle);
	return result;
}

long CCustomViewBase::RestoreContext(long lCtxtHandle)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		lCtxtHandle);
	return result;
}

long CCustomViewBase::SaveData()
{
	long result;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

void CCustomViewBase::SetEditMode(long lMode)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 lMode);
}

long CCustomViewBase::SelectObjectByPath(LPCTSTR szObjectPath)
{
	long result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		szObjectPath);
	return result;
}

void CCustomViewBase::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}
