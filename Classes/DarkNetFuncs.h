#ifndef DARK_NET_FUNCS
#define DARK_NET_FUNCS

#define NETWORK_BUFFER_LENGTH 1024
#define CONNECTION_REQUEST    "hello"
#define CONNECTION_FORMED     "yes"
#define SERVER_FULL           "no slot"

#ifdef WIN32
#include <WinSock2.h>
typedef sockaddr_in SocketAddress;
#endif

#ifdef _XBOX
#include <xtl.h>
typedef sockaddr_in SocketAddress;

const int DARKNET_X360	 = atoi("DARKNET_X360");

//Holds information of XBox Network Layer.
struct XNetInfo
{
	int			m_darknet_xId;
	XNADDR		m_xnaddr;
	XNKID		m_xnkid;
	XNKEY		m_xnkey;
};

//BSD : inet_ntoa functionality for XBox
static char* inet_ntoa(struct in_addr in);
typedef XNetInfo PacketHeader;
#endif

struct Connection
{
  SocketAddress address;
  char			NetworkData[NETWORK_BUFFER_LENGTH];
  void*			pTag;
};

namespace DarkNet
{ 
  enum BlockingMode
  {
    eNonBlocking,
    eBlocking
  };

  int				InitWSA();
  void				DestroyWSA();
  int				CreateUDPSocket();
  void				CloseSocket(int sd);
  char*				GetIp(SocketAddress *addr);
  //Specifying Null for ip, will accept connections from Any Address
  void				CreateSockAddr(SocketAddress& addr, char *ip, int portNum);
  int				Bind(int sd, SocketAddress& addr);
  void				SetBlockingMode(int sd, BlockingMode eMode);
  int				Recieve(int sd, char *buffer, size_t buffSize, SocketAddress &address);
  int Send(int sd, char *buffer, int buffSize, SocketAddress &address);
  int				SetSocketOption(int sd, int option, int value);
  int Broadcast(int sd, int portNum,char* message, int buffSize, SocketAddress &addr);

#if defined _XBOX
  bool				GetXBoxAddr(XNADDR *pXnAddr);
  PacketHeader ExtractHeader(char *buffer,unsigned int size);
  void				IncludeHeader(PacketHeader &header, char *buffer);
  bool				IsBufferValid(char *buffer);
  SocketAddress		GetSocketAddress(XNADDR xAddr, XNKID kID);
#endif //_XBOX
}

#endif