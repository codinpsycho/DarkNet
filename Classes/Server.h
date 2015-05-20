#pragma once
#include "DarkNetFuncs.h"
#include <vector>


typedef void (*OnConnectionFound)(void* params);
typedef void (*OnDataRecieved)(char* data, void* tag);

class Server
{
  Connection				CreateConnection(Address *addr);
  bool						Init();  
  bool						ConnectionExists(Address *addr);
  Connection*				GetConnection(std::string);
  void						DispatchNetworkData();
  void						StoreData(Connection *con, char *data);
  void						AddNewConnection(Address &addr);
  bool						IsNewConnection(char *msg);    

  std::vector<Connection>	m_connections;  
  
  unsigned int				m_socket;  
  int						m_portNum;
  unsigned int				m_maxClients;
  bool						m_isMultithreaded;
  HANDLE				    m_mutex;
public:
  Server(void);
  virtual ~Server(void);
  
  Address				m_serverAddr;

  DWORD						m_threadID;
  volatile bool				ShouldListen;
  std::string				ConnectionMessage;
  OnConnectionFound			ConnCallbackFunc;
  OnDataRecieved			DataCallbackFunc;  
  
  //Dont call this, strictly INTERNAL  
  void						_ReadNetworkData();

  bool						CreateServer(int portNum, int maxClients);
  void						DestroyServer();  
  void						StartListening(char *message, bool multithread, OnConnectionFound, OnDataRecieved);
  void						Update();

  int						Send(Address &addr, char *msg);
  int Broadcast(char *msg);

  int						SendAll(char *msg);
  Address				GetServeraddress()				{ return m_serverAddr;  }
  UINT						GetMaxClients()                 { return m_maxClients;  }
  int						GetPortNum()                    { return m_portNum;     }
  std::vector<Connection>&	GetConnections()				{ return m_connections; }
  
};