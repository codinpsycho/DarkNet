#ifndef DARK_NET_FUNCS
#define DARK_NET_FUNCS
#define NETWORK_BUFFER_LENGTH 1024
#define CONNECTION_REQUEST    "hello"
#define CONNECTION_FORMED     "yes"
#define SERVER_FULL           "no slot"

#ifdef WIN32
#include <WinSock2.h>
typedef sockaddr_in Address;
typedef addrinfo AddressInfo;
#endif

struct Connection
{
	Address			address;
	char			NetworkData[NETWORK_BUFFER_LENGTH];
	void*			pTag;
};

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
	void			CreateSockAddr(Address& addr, char *ip, int portNum);	//Specifying Null for ip, will accept connections from Any Address(INADDR_ANY)
	int				Bind(int sd, Address& addr);
	void			SetBlockingMode(int sd, eBlockingMode eMode);
	int				Recieve(int sd, char *buffer, size_t buffSize, Address &address);
	int				Send(int sd, char *buffer, int buffSize, Address &address);
	int				SetSocketOption(int sd, int option, int value);
	int				Broadcast(int sd, int portNum, char* message, int buffSize, Address &addr);
	int				GetHostName(char *name, size_t len);
	int				GetAddressInfo(char *service_name, char *node_name, eSocketType type, AddressInfo **addr_info);
	void			FreeAddressInfo();
	char*			GetIp(Address *addr);
	void			FillIp(char *ip_address, Address *addr);
}

#endif