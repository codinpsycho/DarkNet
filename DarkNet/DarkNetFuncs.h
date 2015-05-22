#ifndef DARK_NET_FUNCS
#define DARK_NET_FUNCS

#ifdef WIN32
#include <WinSock2.h>
typedef sockaddr_in SockAddr;
typedef addrinfo AddressInfo;
#endif


namespace DarkNet
{ 
	enum eBlockingMode
	{
		NonBlocking,
		Blocking
	};
  
	enum eSocketType
	{
		UDP,
		TCP
	};

	int				InitNetwork();
	void			DestroyNetwork();
	int				CreateSocket(eSocketType _type);
	int				CloseSocket(int sd);
	void			CreateSockAddress(SockAddr& addr, char *ip, int portNum);	//Specifying Null for ip, will accept connections from Any Address(INADDR_ANY)
	int				Bind(int sd, SockAddr& addr);
	void			SetBlockingMode(int sd, eBlockingMode eMode);
	int				RecieveFrom(int sd, char *buffer, size_t buffSize, SockAddr &rev_from);
	int				SendTo(int sd, const char *buffer, int buffSize, SockAddr &send_to);
	int				SetSocketOption(int sd, int option, int value);
	int				Broadcast(int sd, char* message, int buffSize, SockAddr &addr);
	int				GetHostName(char *name, size_t len);
	int				GetAddressInfo(char *service_name, char *node_name, eSocketType type, AddressInfo **addr_info);
	void			FreeAddressInfo();
	void			GetIp(SockAddr *addr, char *ip_address, size_t len);
	void			FillIp(char *ip_address, SockAddr *addr);
}

#endif