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

	Address::Address()
	{
		memset(&addr, 0, sizeof(SockAddr));
	}

	Address::Address(int port_num)
	{
		CreateSockAddress(addr, NULL, port_num);
	}

	Address::Address(char *ip, int port_num)
	{
		CreateSockAddress(addr, ip, port_num);
	}

	Address::~Address()
	{
		memset(&addr, 0, sizeof(SockAddr));
	}
	
	Message::Message()
	{
		memset(buffer, 0, DN_NETWORK_BUFFER_LENGTH);
	}

	Message::Message(const char* _data)
	{
		memset(buffer, 0, DN_NETWORK_BUFFER_LENGTH);		
		strcpy_s(buffer, DN_NETWORK_BUFFER_LENGTH, _data);
	}

	Message::~Message()
	{
		memset(buffer, 0, DN_NETWORK_BUFFER_LENGTH);
	}

	Packet::Packet()
	{}

	Packet::Packet(char* msg, char* ip, int port_num) : msg(msg), address(ip, port_num)
	{}

	Packet::Packet(char* msg, Address addr) : msg(msg), address(addr)
	{}

}