#include "Log.h"
#include "DarkNetFuncs.h"

#if defined(WIN32)
#pragma comment(lib, "ws2_32.lib")
#endif

char* inet_ntoa(struct in_addr in)
{
	int firstBit = in.S_un.S_un_b.s_b1;
	int secondBit = in.S_un.S_un_b.s_b2;
	int thirdBit = in.S_un.S_un_b.s_b3;
	int fourthBit = in.S_un.S_un_b.s_b4;

	char ipAddr[256];
	sprintf(ipAddr,"%d.%d.%d.%d",firstBit,secondBit,thirdBit,fourthBit);

	return ipAddr;
}

namespace DarkNet
{

	int InitWSA()
	{

#if defined(_XBOX)
		if(XNetStartup(NULL) != 0)
		{
			LOG.Write("Failed to Startup XNet");
		}
#endif

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

#if defined(_XBOX)
		XNetCleanup();
#endif
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

#if defined _XBOX

	bool GetXBoxAddr(XNADDR *pXnAddr)
	{
		DWORD dwRet;
		do
		{
			dwRet = XNetGetTitleXnAddr( pXnAddr );
		} while( dwRet == XNET_GET_XNADDR_PENDING );

		if( dwRet & XNET_GET_XNADDR_NONE )
		{
			LOG.Write("Failed to find the XNAddr");
			return false;
		}

		return true;
	}

	//modifies the Buffer, by removing the Header
	PacketHeader ExtractHeader( char *buffer,unsigned int size )
	{
		XNetInfo xInfo;	
		memset(&xInfo,0,sizeof(xInfo));

		if(size >= sizeof(XNetInfo))
		{
			memcpy(&xInfo,buffer,sizeof(XNetInfo));	
			if(IsBufferValid((char*)&xInfo))
			{			
				//Now remove the header
				memmove(buffer,buffer + sizeof(XNetInfo),size - sizeof(XNetInfo));			
				return xInfo;
			}
		}

		xInfo.m_darknet_xId = -1;

		return xInfo;
	}

	void IncludeHeader(PacketHeader &header, char *buffer)
	{
		char newBuff[NETWORK_BUFFER_LENGTH];
		memset(newBuff,0,sizeof(newBuff));
		memcpy(newBuff,&header,sizeof(PacketHeader));
		memcpy(newBuff + sizeof(PacketHeader), buffer,strlen(buffer));

		memcpy(buffer, newBuff, sizeof(newBuff));		
	}

	bool IsBufferValid(char *buffer)
	{
		if(((XNetInfo*)buffer)->m_darknet_xId == DARKNET_X360)
			return true;

		return false;
	}

	SocketAddress GetSocketAddress(XNADDR xAddr, XNKID kID)
	{
		SocketAddress addr;

		IN_ADDR in;

		XNetXnAddrToInAddr(&xAddr,&kID,&in);

		addr.sin_addr.s_addr = in.s_addr;
		addr.sin_port = htons(1000);
		addr.sin_family = AF_INET;

		return addr;
	}

#endif

}