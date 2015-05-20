#include "DarkNetFuncs.h"
#include<cstdio>
#include<Ws2tcpip.h>
#include"Log.h"

#if defined(WIN32)
#pragma comment(lib, "ws2_32.lib")
#endif

namespace DarkNet
{
	int InitNetwork()
	{
		WSADATA wsa;
		int res = WSAStartup(MAKEWORD(2,2), &wsa);
		if(res != 0)
		{
			res = WSAGetLastError();
			OUTPUT("WSAStartup failed : %d\n", res);
		}		
		return res;
	}

	void DestroyNetwork()
	{
		WSACleanup();
	}

	int CloseSocket(int sd)
	{
		return closesocket(sd);
	}

	int CreateSocket(eSocketType _type)
	{
		int sd = INVALID_SOCKET;

		switch (_type)
		{
		case DarkNet::UDP:			
			
			sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			if (sd == INVALID_SOCKET)
			{
				sd = WSAGetLastError();
				OUTPUT("socket() Failed %d\n", sd);
			}
			break;
		case DarkNet::TCP:
			//ToDo
			break;
		default:
			break;
		}

		return sd;
	}


	/*
	For Servers you would want to use null for IP as they can recieve from any client
	*/ 
	void CreateAddress(Address& addr, char *ip, int portNum)
	{
		memset(&addr, 0, sizeof(Address));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(portNum);
		if (!ip)
		{
			addr.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			addr.sin_addr.s_addr = inet_addr(ip);
		}
	}

	int Bind(int sd, Address& addr)
	{
		int res = ::bind(sd, (sockaddr*)&addr, sizeof(Address));
		if(res == SOCKET_ERROR)
		{
			res = WSAGetLastError();
			OUTPUT("bind() failed : %d\n", res);
		}

		return res;
	}

	void SetBlockingMode(int sd, eBlockingMode eMode)
	{
		ULONG mode = -1;
		switch (eMode)
		{
		case NonBlocking:
			mode = 1;
			ioctlsocket(sd, FIONBIO, &mode);
			break;
		case Blocking:
			mode = 0;
			ioctlsocket(sd, FIONBIO, &mode);
			break;
		}
	}

	int RecieveFrom(int sd, char *buffer, size_t buffSize, Address &recv_from)
	{
		if(buffer)
		{			
			int length = sizeof(Address);
			int buff_recv = recvfrom(sd, buffer, buffSize, 0, (sockaddr*)&recv_from, &length);

			if (buff_recv > 0)
			{
				buffer[buff_recv] = '\0';
			}
			
			return buff_recv;
		}    

		return -1;
	}

	int SendTo( int sd, const char *buffer, int buffSize, Address &send_to )
	{
		if(buffer)
		{
			if(buffer > 0)
				return sendto(sd, buffer, buffSize, 0, (sockaddr*)&send_to, sizeof(Address));
		}
		return -1;
	}

	int SetSocketOption(int sd, int option, int value)
	{
		return setsockopt(sd,SOL_SOCKET,option,(char*)&value,sizeof(int));
	}

	int Broadcast( int sd, int portNum,char* message, int buffSize, Address &addr )
	{
		int broadcast = 1;    
		setsockopt(sd, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(int));
		int bytes_sent = SendTo(sd,message,buffSize,addr);
		broadcast = 0;
		setsockopt(sd, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(int));
		return bytes_sent;
	}

	void GetIp(Address *addr, char *ip_address, size_t len)
	{		
		inet_ntop(addr->sin_family, &(addr->sin_addr), ip_address, len);		
	}

	void FillIp(char *ip_address, Address *addr)
	{		
		inet_pton(AF_INET, ip_address, &(addr->sin_addr));
	}

	int GetHostName(char *host_name, size_t name_len)
	{
		int ret = gethostname(host_name, name_len);
		if (ret != 0)
		{
			ret = WSAGetLastError();
		}

		return ret;
	}

	/*
	node_name		:	IP address or "www.example.com"
	service_name	:	port num or "http"
	eSocketType		:	UDP/TCP
	AddressInfo		:	pass a null ptr

	Do remember to free AdressInfo(output) after using
	*/
	int GetAddressInfo(char *node_name, char *service_name, eSocketType type, AddressInfo **output)
	{
		addrinfo hints;
		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;

		switch (type)
		{
		case DarkNet::UDP:
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_protocol = IPPROTO_UDP;
			break;
		case DarkNet::TCP:
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			break;
		default:
			break;
		}
		
		int ret = getaddrinfo(node_name, service_name, &hints, output);
		if (ret != 0)
		{
			ret = WSAGetLastError();
			OUTPUT("getaddrinfo failed with error code %d", ret);
		}

		return ret;
	}

	void FreeAddressInfo(addrinfo *info)
	{
		freeaddrinfo(info);
	}
}