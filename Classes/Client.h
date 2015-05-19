#pragma once
#include "DarkNetFuncs.h"
//#include "Server.h"

typedef void(*ConnectionFormedFunc)(void*);
typedef void(*DataRecievedFunc)(char*,void*);

class Client
{
  int							m_socket;  
  int							m_portNum;
  bool							m_isMultithread;
  bool							m_isConnected;
  
  Connection					m_server;   //Connection with server
  HANDLE						m_mutex;
  DWORD							m_threadID;  
  
  bool							IsServer(SocketAddress &addr);
  void							StoreData(char *msg);
  void							OnConnectionFormed(SocketAddress &addr);
  void							DispatchNetworkData();
public:
  Client(void);
  virtual ~Client(void);
  
  //INTERNAL DO NOT MODIFY
  ConnectionFormedFunc			_ConnCalbackFunc;
  DataRecievedFunc				_DataCallbackFunc;  
  volatile bool					IsAlive;
  void							_ReadNetworkData();

  int							Send(char *msg);
  bool							Start(int portNum, bool multithread, ConnectionFormedFunc connfunc, DataRecievedFunc datarecvFunc);
  void							Stop();
  int							Broadcast(char *msg);
  bool							IsConnected()   {return m_isConnected;  }
  void							Update();
  void							Connect(char *ip, char* message);
  void							Disconnect(); 

};

