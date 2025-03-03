//
// DWNSOCK2.INC	Macros for dynamically linking to WinSock 2
//

#ifdef DWINSOCK_GLOBAL
	#undef DYNAMICWINSOCK
	#define DYNAMICWINSOCK(name, ptr) ptr p_##name
#endif

#ifdef DWINSOCK_EXTERN
	#undef DYNAMICWINSOCK
	#define DYNAMICWINSOCK(name, ptr) extern ptr p_##name
#endif

#ifdef DWINSOCK_GETPROCADDRESS
	#undef DYNAMICWINSOCK
	#define DYNAMICWINSOCK(name, ptr)							\
				p_##name = (ptr)GetProcAddress(hndlWinSock, #name);	\
				if (p_##name == NULL) fOK = FALSE
#endif

	DYNAMICWINSOCK(WSAGetOverlappedResult, LPFN_WSAGETOVERLAPPEDRESULT);
	DYNAMICWINSOCK(WSAIoctl, LPFN_WSAIOCTL);
	DYNAMICWINSOCK(WSARecvFrom, LPFN_WSARECVFROM);
	DYNAMICWINSOCK(WSASendTo, LPFN_WSASENDTO);
	DYNAMICWINSOCK(WSASocketA, LPFN_WSASOCKETA);
	DYNAMICWINSOCK(WSASocketW, LPFN_WSASOCKETW);
	DYNAMICWINSOCK(WSAEnumProtocolsA, LPFN_WSAENUMPROTOCOLSA);
	DYNAMICWINSOCK(WSAEnumProtocolsW, LPFN_WSAENUMPROTOCOLSW);

