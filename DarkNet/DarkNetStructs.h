#ifndef DARK_NET_STRUCTS
#define DARK_NET_STRUCTS

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
		SockAddr address;
		Address(int port_num);
		Address(char *ip, int port_num);
		~Address();
	};
}

#endif