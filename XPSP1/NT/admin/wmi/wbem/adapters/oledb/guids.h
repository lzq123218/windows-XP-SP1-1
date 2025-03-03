/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Microsoft WMI OLE DB Provider 
// (C) Copyright 1999 Microsoft Corporation. All Rights Reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _GUIDS_H_
#define _GUIDS_H_


// {FD8D9C02-265E-11d2-98D9-00A0C9B7CBFE}
DEFINE_GUID(CLSID_WMIOLEDB,				0xfd8d9c02, 0x265e, 0x11d2, 0x98, 0xd9, 0x0, 0xa0, 0xc9, 0xb7, 0xcb, 0xfe);
DEFINE_GUID(CLSID_WMIOLEDB_ROOTBINDER,  0xcdcedb81, 0x5fec, 0x11d3, 0x9d, 0x1c, 0x0, 0xc0, 0x4f, 0x5f, 0x11, 0x64);
// {E14321B2-67C0-11d3-B3B4-00104BCC48C4}
DEFINE_GUID(CLSID_WMIOLEDB_ENUMERATOR,  0xe14321b2, 0x67c0, 0x11d3, 0xb3, 0xb4, 0x0, 0x10, 0x4b, 0xcc, 0x48, 0xc4);
//{80C4A61D-CB78-46fd-BD8F-8BF45BE46A4C}
DEFINE_GUID(CLSID_WMIOLEDB_ERRORLOOOKUP,  0x80C4A61D, 0xCB78, 0x46fd, 0xBD, 0x8F, 0x8B, 0xF4, 0x5B, 0xE4, 0x6A, 0x4C);

// {21B2028F-AD0A-11d3-B3CB-00104BCC48C4}
DEFINE_GUID(DBGUID_WQL, 0x21b2028f, 0xad0a, 0x11d3, 0xb3, 0xcb, 0x0, 0x10, 0x4b, 0xcc, 0x48, 0xc4);
// {18EC17EA-14D9-47fc-9722-2DCD8A1C3D70}
DEFINE_GUID(DBGUID_LDAP,0x18ec17ea, 0x14d9, 0x47fc, 0x97, 0x22, 0x2d, 0xcd, 0x8a, 0x1c, 0x3d, 0x70);
// {3718AA52-5F6E-4c2e-AAAE-383E69C0C928}
DEFINE_GUID(DBGUID_LDAPSQL,0x3718aa52, 0x5f6e, 0x4c2e, 0xaa, 0xae, 0x38, 0x3e, 0x69, 0xc0, 0xc9, 0x28);

// {D75984C0-8FFF-4d4c-835A-26BFAC6C01B7}
DEFINE_GUID(DBGUID_WMI_METHOD,0xd75984c0, 0x8fff, 0x4d4c, 0x83, 0x5a, 0x26, 0xbf, 0xac, 0x6c, 0x1, 0xb7);

// {CF60E9AB-F782-413c-B25D-DECE26E172D0}
DEFINE_GUID(DBOBJECT_WMIINSTANCE, 0xcf60e9ab, 0xf782, 0x413c, 0xb2, 0x5d, 0xde, 0xce, 0x26, 0xe1, 0x72, 0xd0);

 
DEFINE_GUID(DBPROPSET_WMIOLEDB_DBINIT, 0xdd497a71,0x9628,0x11d3,0x9d,0x5f,0x0,0xc0,0x4f,0x5f,0x11,0x64);
DEFINE_GUID(DBPROPSET_WMIOLEDB_ROWSET, 0x8d16c220,0x9bbb,0x11d3,0x9d,0x65,0x0,0xc0,0x4f,0x5f,0x11,0x64);
DEFINE_GUID(DBPROPSET_WMIOLEDB_COLUMN, 0x3ed51791,0x9c76,0x11d3,0x9d,0x66,0x0,0xc0,0x4f,0x5f,0x11,0x64);
DEFINE_GUID(DBPROPSET_WMIOLEDB_COMMAND,0xda0ff63c,0xad10,0x11d3,0xb3,0xcb,0x0,0x10,0x4b,0xcc,0x48,0xc4);

#endif


