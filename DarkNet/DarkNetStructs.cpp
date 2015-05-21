#include "DarkNetFuncs.h"
#include "DarkNetStructs.h"

namespace DarkNet
{	
	UDPSocket::UDPSocket()
	{
		sd = CreateSocket(eSocketType::UDP);
	}

	UDPSocket::~UDPSocket()
	{
		CloseSocket(sd);
	}

	Address::Address(int port_num)
	{
		CreateSockAddress(address, NULL, port_num);
	}

	Address::Address(char *ip, int port_num)
	{
		CreateSockAddress(address, ip, port_num);
	}

	Address::~Address()
	{
		memset(&address, 0, sizeof(SockAddr));
	}
	
}