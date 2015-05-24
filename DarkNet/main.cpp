#include"DarkNetFuncs.h"
#include "DarkNetwork.h"
#include<iostream>
#include "Log.h"

using namespace DarkNet;

//Callbacks
void DataRecieved(Packet* pck)
{
	char buff[DN_NETWORK_BUFFER_LENGTH];
	sprintf_s(buff, "Msg : %s | Bytes recieved = %d\n", pck->buff.buffer, strlen(pck->buff.buffer));

	std::cout << buff;
	if (strcmp(pck->buff.buffer, "quit") == 0)
		exit(0);
}

void ConnectionFound(void* params)
{

}

int main()
{	
	DarkNetwork::Instance().Start(ConnectionFound, DataRecieved);
	char input[DN_NETWORK_BUFFER_LENGTH];
	char choice;
	
	std::cout << "1. Listen\n";
	std::cout << "2. Send to localhost\n";
	std::cout << "3. Broadcast\n";
	std::cout << "4. Host\n";
	std::cin >> choice;

	switch (choice)
	{
	case '1':
		while (true)
		{
			Sleep(10);
			DarkNetwork::Instance().Update();
		}
		break;
	case '2':
	{
		Address addr(DN_ADDRESS_LOCALHOST, DN_NETWORK_PORT_NUM);
		while (true)
		{
			Sleep(10);
			std::cin >> input;			
			Packet pck(input, addr);
			int bytes_sent = DarkNetwork::Instance().Send(pck);
			char buff[64];
			sprintf_s(buff, "bytes_sent = %d\n", bytes_sent);
			std::cout << buff;
		}
	}
		
		break;
	case '3':
		while (true)
		{
			Sleep(10);
			std::cin >> input;			
			DarkNetwork::Instance().Broadcast(Buffer(input));
		}		
		break;
	case '4':
		DarkNetwork::Instance().Host(4);
		while (true)
		{
			Sleep(10);
			DarkNetwork::Instance().Update();
			Peer *p = DarkNetwork::Instance().GetPeer(Address("127.0.0.1", DN_NETWORK_PORT_NUM));
			if (p)
			{
				p->SetOutputBuffer("wtf");
			}
			
		}		
		break;
	}
	

	system("pause");
	return 0;
}