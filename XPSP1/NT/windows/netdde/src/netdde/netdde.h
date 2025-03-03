#define GUI_ROUTES

#define IDM_NODENAME			101
#define IDM_NETINTF_PARMS		102
#define IDM_DEFAULT_NETINTF		103
#define IDM_ROUTE_INFO			104
#define IDM_ADD_NETINTF			105
#define IDM_DELETE_NETINTF		106
#define IDM_CLOSE_CONNECTION		107
#define IDM_CONNECT			108
#define IDM_NETINTFS			109
#define IDM_SECURITY_FILE		111
#define IDM_CLOSE_ROUTE			112
#define IDM_PREFERENCES			113
#define IDM_PHYSICAL_CONNECTIONS	114
#define IDM_VALIDATE_FILES		115
#define IDM_SHOW_LOGGER			116
#define IDM_CLOSE			117
#define IDM_SHOW_FONT       118
#define IDM_CLOSE_ALL_CONNECTIONS	119

#define IDM_CONFIG_IF       120

#define IDM_HELP_INDEX			291
#define IDM_HELP_HELP			292
#define IDM_ABOUT			293

#define MAX_NETINTFS 8

#define IDM_SHOW_DDER			0x100
#define IDM_DEBUG_DDE			0x110
#define IDM_LOG_INFO			0x120
#define IDM_LOG_DDE_PKTS		0x130
#define IDM_DEBUG_NETDDE		0x140
#define IDM_DEBUG_NETDDE_MEMORY		0x150
#define IDM_DEBUG_INTF			0x160
#define IDM_DEBUG_INTF_MAX		(IDM_DEBUG_INTF + (MAX_NETINTFS<<4))

#define IDM_SHOW_PKTZ			401
#define IDM_SHOW_ROUTER			402
#define IDM_SHOW_IPC			404
#define IDM_SHOW_STATISTICS		405
#define IDM_SHOW_ROUTER_THRU		406

#define CI_ND_NAME			131
#define CI_EDIT				132
#define CI_DFLT_ALLOW			133
#define CI_DFLT_RUN			134
#define CI_DFLT_DISCONNECT		135
#define CI_DFLT_DELAY			136
#define CI_TIME_SLICE           137

#define CI_VERSION			200

#define CI_DEST_NAME			300
#define CI_ROUTE_INFO			301
#define CI_DEFAULTS			302
#define CI_ALLOW_ADVISE			304
#define CI_ALLOW_REQUEST		305
#define CI_ALLOW_POKE			306
#define CI_ALLOW_EXECUTE		307
#define CI_DFLT_ROUTE			308
#define CI_COMBO_NAME			309
#define CI_CLOSE_ALL			310
#define CI_LOG_PERM_VIOL		311
#define CI_LOG_EXEC_FAIL		312

#define CI_RTNAME   510
#define CI_RTDELETE 511
#define CI_RTADD    512
#define CI_RTMODIFY 513
#define CI_RTPATH   514
#define CI_RTPATHI  5115

#define CI_HELP				10001

#define CI_SERIAL		141
#define CI_NETBIOS		142
#define CI_NMTCPIP		143
#define CI_FTCPIP       144
#define CI_DECNET		145
#define CI_NOVELL		146


/**** Help Contexts ****/
#define HC_NODE_NAME			0x0200
#define HC_ROUTE_INFO			0x0201
#define HC_CONNECT_OPEN			0x0202
#define HC_NETINTF			0x0203
#define HC_SECURITY_INFO		0x0204
#define HC_PREFERENCES			0x0205
#define HC_CONNECTION_INFO		0x0206
#define HC_SECURITY_DEFAULT		0x0207
#define HC_CONNECTION_DEFAULT		0x0208
#define HC_ROUTE_DEFAULT		0x0209
#define HC_CLOSE			0x020A
#define HC_ROUTE_CONFIG     0x020B

/*** add any additional lines to netdde.hpj! ***/

BOOL	FAR PASCAL InitApplication( HANDLE );
BOOL	FAR PASCAL InitInstance( HANDLE, int, LPSTR );
LPARAM	FAR PASCAL MainWndProc( HWND, unsigned, WPARAM, LPARAM );
BOOL	FAR PASCAL About( HWND, unsigned, UINT, LONG );
BOOL	FAR PASCAL ProtGetDriverName( LPSTR lpszName, int nMaxLength );

