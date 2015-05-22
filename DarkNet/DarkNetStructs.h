#ifndef DARK_NET_STRUCTS
#define DARK_NET_STRUCTS

#include "DarkNetDefs.h"
#include "DarkNetFuncs.h"

namespace DarkNet
{
	struct UDPSocket
	{
		int sd;
		UDPSocket();
		~UDPSocket();
	};

	struct Address
	{
		SockAddr addr;
		Address();
		Address(int port_num);
		Address(char *ip, int port_num);
		~Address();
	};

	struct Message
	{
		char buffer[DN_NETWORK_BUFFER_LENGTH];
		size_t size()		{ return strlen(buffer);			}
		size_t capacity()	{ return DN_NETWORK_BUFFER_LENGTH;	}
		Message();
		Message(const char* _data);
		~Message();
	};

	struct Packet 
	{
		Message msg;
		Address address;
		Packet();
		Packet(char* msg, char* ip, int port_num);
		Packet(char* msg, Address addr);
		Packet(Message msg, Address addr);
	};

	struct Peer
	{
		Packet	pck;
		void*	pTag;
	};
}

#endif