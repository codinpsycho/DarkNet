#include "Client.h"
#include "Log.h"

#if defined( WIN32) || (_XBOX)
DWORD __stdcall NetworkThread_Client(void *params)
{
  Client *client = reinterpret_cast<Client*>(params);	
  while(client->IsAlive)
  {
    Sleep(10);

    client->_ReadNetworkData();
    //Check for alive server

  }

  return 0;
}
#endif


Client::Client(void)
{
  m_socket = INVALID_SOCKET;  
  m_portNum = 0;
  m_isMultithread = false;
  m_isConnected = false; 
  IsAlive = false;
  m_mutex = INVALID_HANDLE_VALUE;
  m_threadID = 0;  
  memset(&m_server, 0, sizeof(m_server));
  memset(&m_xnetInfo, 0, sizeof(m_xnetInfo));
  _ConnCalbackFunc = NULL;
  _DataCallbackFunc = NULL;
}


Client::~Client(void)
{
  IsAlive = false;
}

bool Client::Start(int portNum, bool multithread, ConnectionFormedFunc connfunc, DataRecievedFunc datarecvFunc)
{
  int res = DarkNet::InitWSA();
  OUTPUT("WSAStartup returned : %d\n", res);

  m_socket = DarkNet::CreateUDPSocket();
  if(m_socket<0)
    return false;
    
  IsAlive = true;
  m_portNum = portNum;
  _ConnCalbackFunc = connfunc;
  _DataCallbackFunc = datarecvFunc;
  m_isMultithread = multithread;
  
  DarkNet::CreateSockAddr(m_server.address,NULL,m_portNum);
  res = DarkNet::Bind(m_socket, m_server.address);
  if(res < 0 )
  {
    OUTPUT("Bind failed\n");    
    return false;
  }

  if(!DarkNet::GetXBoxAddr(&m_xnetInfo.m_xnaddr))
	  OUTPUT("GetXBoxAddr failed\n");

  if(m_isMultithread)
  {
#if defined (WIN32) || (_XBOX)
    m_mutex = CreateMutex(NULL,false,NULL);
    CreateThread(NULL,1024,NetworkThread_Client,this,0,&m_threadID);
#endif
  }

  return true;
}

void Client::Stop()
{
  IsAlive = false;  //Stop the network thread
  m_portNum = 0;
  m_mutex = INVALID_HANDLE_VALUE;
  DarkNet::CloseSocket(m_socket);
  memset(&m_server.address, 0, sizeof(m_server.address) );
  memset(m_server.NetworkData,0,sizeof(m_server.NetworkData));
}

//This is supposed to be called when we decide our server
void Client::Connect(char *ip, char *message)
{  
  SocketAddress addr;
  DarkNet::CreateSockAddr(addr,ip,m_portNum);
  DarkNet::Send(m_socket,message,strlen(message),addr);
}

void Client::Disconnect()
{
  if(!m_isConnected || !IsAlive)
    return;
  
  m_isConnected = false;
  DarkNet::CreateSockAddr(m_server.address,NULL,m_portNum);
  
}

//Client can only send to the server
int Client::Send(char *msg)
{
  if(!m_isConnected)
    return 0;

  char buffer[NETWORK_BUFFER_LENGTH];
  strcpy(buffer,msg);
#if defined(_XBOX)	
  DarkNet::IncludeHeader(m_xnetInfo,buffer);
#endif

  return DarkNet::Send(m_socket,buffer,sizeof(buffer),m_server.address);
}

void Client::DispatchNetworkData()
{
  char buffer[NETWORK_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));

  if(strlen(m_server.NetworkData))
  {
    WaitForSingleObject(m_mutex, INFINITE);
    strcpy(buffer, m_server.NetworkData);
    memset(m_server.NetworkData,0, sizeof(m_server.NetworkData));
    ReleaseMutex(m_mutex);
    _DataCallbackFunc(buffer, &m_server);
  }
}

void Client::Update()
{
  if(!IsAlive)
    return;

  if(!m_isMultithread)
  {
    _ReadNetworkData();
  }  
  DispatchNetworkData();
}

int Client::Broadcast(char *msg)
{
	char buffer[NETWORK_BUFFER_LENGTH];
	strcpy(buffer,msg);
#if defined(_XBOX)	
	DarkNet::IncludeHeader(m_xnetInfo,buffer);
#endif

  SocketAddress addr;
  DarkNet::CreateSockAddr(addr,"255.255.255.255",m_portNum);
  return DarkNet::Broadcast(m_socket, m_portNum, buffer,sizeof(buffer), addr);
}

void Client::_ReadNetworkData()
{
  char buffer[NETWORK_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));  
  SocketAddress addr;
  memset(&addr, 0, sizeof(SocketAddress));
  int bytes_recv = 0;
  
  while(bytes_recv != -1 )
  {
    bytes_recv = DarkNet::Recieve(m_socket,buffer, sizeof(buffer),addr);

#if defined (_XBOX)		
	//Make sure it has XnetInfo as header
	PacketHeader header = DarkNet::ExtractHeader(buffer,bytes_recv);
	if(header.m_darknet_xId == -1)
		continue;
	
	if(!m_isConnected)
		if(XNetRegisterKey(&header.m_xnkid,&header.m_xnkey) != 0)
			OUTPUT("FAILED To Register the key!");

	//Else get the socketaddress
	addr = DarkNet::GetSocketAddress(header.m_xnaddr, header.m_xnkid);
#endif

    if(IsServer(addr))
    {
      StoreData(buffer);      
    }
	else if(strcmp(buffer, CONNECTION_FORMED) == 0)
	{ 
		OnConnectionFormed(addr);
	}     
    else
      _DataCallbackFunc(buffer,&addr);
  }
}

void Client::OnConnectionFormed(SocketAddress &addr)
{
  m_isConnected = true;
  m_server.address = addr;
  m_server.pTag = NULL;
  memset(m_server.NetworkData, 0, sizeof(m_server.NetworkData));

  _ConnCalbackFunc(&m_server);
}

void Client::StoreData(char *msg)
{
  WaitForSingleObject(m_mutex,INFINITE);
  strcpy((char*)m_server.NetworkData,msg);
  ReleaseMutex(m_mutex);
}

bool Client::IsServer(SocketAddress &addr)
{
  std::string serverIP = DarkNet::GetIp(&m_server.address);
  std::string testIP = DarkNet::GetIp(&addr);
  
  if( serverIP.compare(testIP) == 0 )
    return true;

  return false;
}