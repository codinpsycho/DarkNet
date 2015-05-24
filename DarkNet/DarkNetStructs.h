#ifndef DARK_NET_STRUCTS
#define DARK_NET_STRUCTS

#include "DarkNetDefs.h"
#include "DarkNetFuncs.h"

namespace DarkNet
{
	enum eListeningState
	{
		eActive,
		eInactive
	};

	enum eMode
	{
		eHost,
		eClient,
		eIdle
	};

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

	struct Buffer
	{
		char buffer[DN_NETWORK_BUFFER_LENGTH];
		size_t	size()		{ return strlen(buffer);						}
		size_t	capacity()	{ return DN_NETWORK_BUFFER_LENGTH;				}
		void	clear()		{ memset(buffer, 0, DN_NETWORK_BUFFER_LENGTH);	}
		Buffer();
		Buffer(const char* _data);
		bool operator==(const Buffer rhs)
		{
			return !(strcmp(buffer, rhs.buffer));
		}

		Buffer operator+(const Buffer &rhs)
		{
			Buffer result = buffer;
			strcpy_s(&(result.buffer[result.size()]), result.capacity() - result.size(), rhs.buffer);
			return result;
		}
		~Buffer();
	};

	struct Packet 
	{
		Buffer buff;
		Address address;
		Packet();
		Packet(Buffer msg, char* ip, int port_num);		
		Packet(Buffer msg, Address addr);
	};

	struct Peer
	{
		Address address;
		Buffer input_msg;
		Buffer output_msg;
		eMode mode;
		Peer();
		void*	pTag;
		void SetInputBuffer(Buffer input);
		void SetOutputBuffer(Buffer output);
	};
}

#endif