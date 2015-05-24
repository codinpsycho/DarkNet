#ifndef DARK_NET_DEFS_H
#define DARK_NET_DEFS_H

namespace DarkNet
{
#define DN_NETWORK_BUFFER_LENGTH		1024
#define DN_NETWORK_PORT_NUM				5011
#define DN_MSG_REQUEST_DETAILS			"hello"		//Boradcast msg. Client will use to get host's info
#define DN_MSG_REQUEST_CONNECTION		"request"	//Client reqesting connection to Host
#define DN_MSG_NEW_PEER_FORMED			"new_peer"	//Server sends to all informing new peer
#define DN_MSG_CONNECTION_ACCEPTED		"ok"		//Client will receive this msg once host accepts the connection
#define DN_MSG_SERVER_FULL				"no slot"		
#define DN_ADDRESS_LOCALHOST			"127.0.0.1"
#define DN_ADDRESS_BROADCAST			"255.255.255.255"
}

#endif