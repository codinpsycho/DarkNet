#include "Server.h"
#include "Log.h"

#if defined (WIN32) || (_XBOX)
DWORD __stdcall NetworkThread_Server(void *params)
{
	Server *serv = reinterpret_cast<Server*>(params);	
	while(serv->ShouldListen)
	{
		Sleep(10);

		serv->_ReadNetworkData();      

		//Check for dead clients      

	}

	return 0;
}
#endif

Server::Server(void)
{  
	m_socket = INVALID_SOCKET; 
	ShouldListen = false;
	ConnectionMessage = "";
	ConnCallbackFunc = NULL;
	m_isMultithreaded = false;

#if defined(_XBOX)
	memset(&m_xnetInfo,0,sizeof(XNetInfo));
#endif
}


Server::~Server(void)
{
}

//Inits required libraries
bool Server::Init()
{
	int res = DarkNet::InitWSA();
	OUTPUT("WSAStartup returned : %d\n", res);

	m_socket = DarkNet::CreateUDPSocket();
	if(m_socket<0)
		return false;

#if defined (_XBOX)
	if(!DarkNet::GetXBoxAddr(&m_xnetInfo.m_xnaddr))
		return false;	

	if( XNetCreateKey( &m_xnetInfo.m_xnkid, &m_xnetInfo.m_xnkey ) != 0 )
	{
		LOG.Write("Failed to Create a Key");
		return false;
	}

	if(XNetRegisterKey( &m_xnetInfo.m_xnkid, &m_xnetInfo.m_xnkey ) != 0)
	{
		LOG.Write("ERROR REGISTER FAILED!");
		return false;
	}
#endif

	return true;
}

void Server::DestroyServer()
{
	ShouldListen = false;   //This closes the Listener thread  
	DarkNet::CloseSocket(m_socket);

#if defined(_XBOX)
	XNetUnregisterKey(&m_xnetInfo.m_xnkid);
#endif

	m_connections.clear();
	DarkNet::DestroyWSA();
	m_mutex = INVALID_HANDLE_VALUE;
}

bool Server::CreateServer(int portNum, int maxClients)
{
	if(Init())
	{        
		//Create Data Socket
		DarkNet::CreateSockAddr(m_serverAddr, NULL, portNum);
		//Now bind both addr and socket
		int res = DarkNet::Bind(m_socket, m_serverAddr);
		if(res < 0)
		{
			OUTPUT("Bind() failed\n ");
			return false;
		}
		OUTPUT("Server Created\n");    

		m_maxClients = maxClients;

		m_connections.reserve(m_maxClients);
		m_threadID = 0;
		m_portNum = portNum;

#if defined(_XBOX)
		DarkNet::SetSocketOption(m_socket,SO_BROADCAST,1);
#endif
		return true;
	} 

	OUTPUT("Init() Failed\n");
	return false;
}

void Server::StartListening(char *message, bool multithread, OnConnectionFound connectionCallback, OnDataRecieved dataCallback)
{
	ShouldListen = true;
	ConnectionMessage = message;
	ConnCallbackFunc = connectionCallback;
	DataCallbackFunc = dataCallback;
	m_isMultithreaded = multithread;

	if(m_isMultithreaded)
	{
#if defined (WIN32) || (_XBOX)
		m_mutex = CreateMutex(NULL,false,NULL);
		CreateThread(NULL,1024,NetworkThread_Server,this,0,&m_threadID);
#endif
	}  
}

void Server::StoreData(Connection *con, char* data)
{
	WaitForSingleObject(m_mutex,INFINITE);
	strcpy((char*)con->NetworkData, data);
	ReleaseMutex(m_mutex);
}

bool Server::IsNewConnection(char *msg)
{
	return (strcmp((char*)msg,(char*)ConnectionMessage.c_str()) == 0);
}

void Server::AddNewConnection(SocketAddress &addr){    
	Connection con = CreateConnection(&addr);
	m_connections.push_back(con);

#if defined(_XBOX)
	Broadcast(CONNECTION_FORMED);
#else
	Send(addr,CONNECTION_FORMED);
#endif

	ConnCallbackFunc(&con);
}

void Server::_ReadNetworkData()
{
	char buffer[NETWORK_BUFFER_LENGTH];
	memset(buffer, 0, sizeof(buffer));  
	SocketAddress addr;
	memset(&addr, 0, sizeof(SocketAddress));
	int bytes_recv = 0;

	while(bytes_recv != -1 )
	{

		//Get IP Address from Client.
		bytes_recv = DarkNet::Recieve(m_socket,buffer, sizeof(buffer),addr);
		
		if(bytes_recv <= 0)
			continue;

#if defined (_XBOX)		
		//Make sure it has XnetInfo as header
 		PacketHeader header = DarkNet::ExtractHeader(buffer,bytes_recv);
		if(header.m_darknet_xId == -1)
			continue;

		//Else get the socketaddress
		addr = DarkNet::GetSocketAddress(header.m_xnaddr, m_xnetInfo.m_xnkid);
#endif

		if(ConnectionExists(&addr))
		{
			Connection *con  = GetConnection(DarkNet::GetIp(&addr));
			StoreData(con, buffer);
		}
		else if(IsNewConnection(buffer))
		{
			if((m_connections.size() < m_maxClients))
				AddNewConnection(addr);
			else
			{
#if defined (_XBOX)	
				Broadcast(SERVER_FULL);
#else
				Send(addr,SERVER_FULL);
#endif
			}
		}
		else
		{
			DataCallbackFunc(buffer,&addr);
		}
		//else Reject[Trash data]
	}  

}

//Distributes collected network data locally
void Server::DispatchNetworkData()
{
	char buffer[NETWORK_BUFFER_LENGTH];
	memset(buffer,0,sizeof(buffer));
	for(unsigned int i = 0, end = m_connections.size(); i < end; ++i)
	{
		if(strlen(m_connections[i].NetworkData))
		{
			WaitForSingleObject(m_mutex,INFINITE);
			strcpy(buffer,m_connections[i].NetworkData);
			memset(m_connections[i].NetworkData,0,sizeof(m_connections[i].NetworkData));
			ReleaseMutex(m_mutex);
			DataCallbackFunc(buffer, &m_connections[i]);
		}
	}
}

void Server::Update()
{
	if(!ShouldListen)
		return;

	if(!m_isMultithreaded)
	{    
		_ReadNetworkData();

		//Check for dead clients()

	}//ELSE all this will happen in Network thread

	DispatchNetworkData();
}

int Server::Send(SocketAddress &addr, char *msg)
{
	char buffer[NETWORK_BUFFER_LENGTH];
	strcpy(buffer,msg);
#if defined(_XBOX)	
	DarkNet::IncludeHeader(m_xnetInfo,buffer);
#endif

	return DarkNet::Send(m_socket,buffer,sizeof(buffer),addr);
}

int Server::Broadcast(char *msg)
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

Connection Server::CreateConnection(SocketAddress *addr)
{
	Connection con;  
	con.address = *addr;
	con.pTag = NULL;
	memset(con.NetworkData, 0, sizeof(con.NetworkData));  

	return con;
}

bool Server::ConnectionExists(SocketAddress *addr)
{  
	std::string ip(DarkNet::GetIp(addr));

	for (unsigned int i = 0, end = m_connections.size(); i < end; ++i)
	{
		std::string tempIP = DarkNet::GetIp(&m_connections[i].address);
		if(ip.compare(tempIP)  == 0)
			return true;
	}
	return false;
}

Connection* Server::GetConnection(std::string ip)
{  
	for (unsigned int i = 0, end = m_connections.size(); i < end; ++i)
	{
		std::string tempIP = DarkNet::GetIp(&m_connections[i].address);
		if(ip.compare(tempIP) == 0)
			return &m_connections[i];
	}
	OUTPUT("No such connection found\n");

	return NULL;
}