#include"DarkNetFuncs.h"
#include<iostream>
#include "Log.h"

using namespace DarkNet;

void CreateServer()
{
	Address addr;	
	CreateAddress(addr, NULL, 5011);
	int sd = CreateSocket(DarkNet::UDP);
	Bind(sd, addr);

	char buff[NETWORK_BUFFER_LENGTH];	
	Address from;
	RecieveFrom(sd, buff, NETWORK_BUFFER_LENGTH, from);

	char ip[512];
	GetIp(&from, ip, 512);
	
	OUTPUT("Recieved from : %s", ip);
	OUTPUT(buff);
	CloseSocket(sd);
}

void CreateClient()
{	
	Address addr;
	CreateAddress(addr, "127.0.0.1", 5011);
	int sd = CreateSocket(DarkNet::UDP);

	//Note: Bind is not necessary for client, SendTo will bind automatically
	char buff[NETWORK_BUFFER_LENGTH];
	SendTo(sd, "Testing Testing", 16, addr);
	OUTPUT(buff);
	CloseSocket(sd);
}

int main()
{
	OUTPUT("%d",strlen("255.255.255.255"));
	DarkNet::InitNetwork();	

	int input;	
	std::cout << "Press 1 for Server\n";
	std::cout << "Press 2 for client\n";
	std::cin >> input;

	switch (input)
	{
	case 1:
		CreateServer();
		break;
	case 2:
		CreateClient();
		break;
	}

	DarkNet::DestroyNetwork();
	system("pause");
	return 0;
}