#include"DarkNetFuncs.h"
#include "DarkNetStructs.h"
#include "DarkNetwork.h"
#include<iostream>
#include "Log.h"

using namespace DarkNet;

//Callbacks
void DataRecieved(Packet* pck, void* tag)
{
	
	char buff[DN_NETWORK_BUFFER_LENGTH];
	sprintf_s(buff, "Msg : %s | Bytes recieved = %d\n", pck->msg.buffer, strlen(pck->msg.buffer));

	std::cout << buff;
	if (strcmp(pck->msg.buffer, "quit") == 0)
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
		Address addr(, DN_NETWORK_PORT_NUM);
		while (true)
		{
			Sleep(10);
			std::cin >> input;
			Packet pck(input, addr);
			int bytes_sent = DarkNetwork::Instance().Send(&pck);
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
			DarkNetwork::Instance().Broadcast(input);
		}		
		break;
	}
	

	system("pause");
	return 0;
}