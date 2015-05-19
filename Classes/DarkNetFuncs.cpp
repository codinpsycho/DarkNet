#include "Log.h"
#include "DarkNetFuncs.h"

#if defined(WIN32)
#pragma comment(lib, "ws2_32.lib")
#endif

namespace DarkNet
{
	int InitWSA()
	{
		WSADATA wsa;
		int res = WSAStartup(MAKEWORD(2,2), &wsa);
		if(res != 0)
		{
			res = WSAGetLastError();
			LOG.Write("WSAStartup failed : %d\n", res);
		}

		LOG<<"WSA initialized\n";
		return res;
	}

	void DestroyWSA()
	{
		WSACleanup();
	}

	void CloseSocket(int sd)
	{
		closesocket(sd);
	}

	int CreateUDPSocket()
	{
		//Open a datagram socket
		int sd = INVALID_SOCKET;  
		sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if(sd == INVALID_SOCKET)
		{
			sd = WSAGetLastError();
			LOG.Write("socket() Failed %d\n", sd);
			return -1;
		}

		return sd;
	}

	void CreateSockAddr(SocketAddress& addr, char *ip, int portNum)
	{
		memset(&addr, 0, sizeof(sockaddr_in));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(portNum);   
		if(!ip)
			addr.sin_addr.s_addr = INADDR_ANY;
		else
			addr.sin_addr.s_addr = inet_addr(ip);
	}

	int Bind(int sd, sockaddr_in& addr)
	{
		int res = ::bind(sd, (sockaddr*)&addr, sizeof(sockaddr_in));
		if(res == SOCKET_ERROR)
		{
			res = WSAGetLastError();
			LOG.Write("bind() failed : %d\n", res);
			return -1;
		}

		return 1;
	}

	void SetBlockingMode(int sd, BlockingMode eMode)
	{
		ULONG mode = -1;
		switch (eMode)
		{
		case eNonBlocking:
			mode = 1;
			ioctlsocket(sd, FIONBIO, &mode);
			break;
		case eBlocking:
			mode = 0;
			ioctlsocket(sd, FIONBIO, &mode);
			break;
		}
	}

	int Recieve(int sd, char *buffer, size_t buffSize, SocketAddress &addr)
	{
		if(buffer)
		{
			if(buffSize > 0)
			{
				int length = sizeof(sockaddr_in);
				int buff_recv = recvfrom(sd, buffer,buffSize,0, (sockaddr*)&addr,&length);

				if(buff_recv > 0) 
					buffer[buff_recv] = '\0';
				return buff_recv;
			}      
		}    

		return -1;
	}

	int Send( int sd, char *buffer, int buffSize, SocketAddress &address )
	{
		if(buffer)
		{
			if(buffer > 0)
				return sendto(sd, buffer, buffSize, 0, (sockaddr*)&address, sizeof(SocketAddress) );
		}
		return -1;
	}

	int SetSocketOption(int sd, int option, int value)
	{
		return setsockopt(sd,SOL_SOCKET,option,(char*)&value,sizeof(int));
	}

	int Broadcast( int sd, int portNum,char* message, int buffSize, SocketAddress &addr )
	{
		int broadcast = 1;    
		setsockopt(sd, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(int));
		int bytes_sent = Send(sd,message,buffSize,addr);
		//broadcast = 0;
		//setsockopt(sd, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(int));
		return bytes_sent;
	}

	char* GetIp(SocketAddress *addr)
	{
		return inet_ntoa(addr->sin_addr);
	}
}