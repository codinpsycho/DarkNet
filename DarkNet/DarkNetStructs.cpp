#include "DarkNetStructs.h"

namespace DarkNet
{	
	UDPSocket::UDPSocket()
	{
		sd = CreateSocket(eSocketType::eUDP);
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
	
	Buffer::Buffer()
	{
		memset(buffer, 0, DN_NETWORK_BUFFER_LENGTH);
	}

	Buffer::Buffer(const char* _data)
	{
		memset(buffer, 0, DN_NETWORK_BUFFER_LENGTH);		
		strcpy_s(buffer, DN_NETWORK_BUFFER_LENGTH, _data);
	}

	Buffer::~Buffer()
	{
		memset(buffer, 0, DN_NETWORK_BUFFER_LENGTH);
	}
		
	Packet::Packet()
	{}

	Packet::Packet(Buffer msg, char* ip, int port_num) : buff(msg), address(ip, port_num)
	{}

	Packet::Packet(Buffer msg, Address addr) : buff(msg), address(addr)
	{}

	Peer::Peer() : mode(eClient)
	{}

	void Peer::SetInputBuffer(Buffer input)
	{
		input_msg.clear();
		input_msg = input;
	}

	void Peer::SetOutputBuffer(Buffer output)
	{
		output_msg.clear();
		output_msg = output;
	}

}