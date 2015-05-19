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
}

#endif