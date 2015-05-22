#include "DarkNetwork.h"
#include "DarkNetFuncs.h"
#include "DarkNetStructs.h"
#include "Log.h"


namespace DarkNet
{
	DarkNetwork* DarkNetwork::m_instance = nullptr;
	
	DarkNetwork::DarkNetwork()
	{
		
	}


	DarkNetwork::~DarkNetwork()
	{

	}

	void DarkNetwork::Start(OnConnectionFound connection_callback, OnDataRecieved data_callback)
	{
		if (connection_callback == NULL || data_callback == NULL)
		{
			OUTPUT("connection_callback or data_callback cant be null, Cannot start");
			return;
		}

		if (DarkNet::InitNetwork() == 0)
		{
			OUTPUT("DarkNet initialized\n");
		}

		m_socket = new UDPSocket();
		m_address = new Address(DN_NETWORK_PORT_NUM);
		int ret = DarkNet::Bind(m_socket->sd, m_address->addr);
		if (ret != 0)
		{
			OUTPUT("Bind failed %d. Cannot listen for incoming packets", ret);
		}
		DarkNet::SetBlockingMode(m_socket->sd, DarkNet::NonBlocking);

		m_connection_callback = connection_callback;
		m_data_callback = data_callback;
	}

	void DarkNetwork::Stop()
	{
		DestroyNetwork();
		delete m_socket;
		delete m_address;

		m_connection_callback	= nullptr;
		m_data_callback			= nullptr;
	}

	void DarkNetwork::Update()
	{
		Listen();
	}

	int DarkNetwork::Listen()
	{
		Packet pck;
		int bytes_rcvd = 0;
		while (bytes_rcvd != -1)
		{
			bytes_rcvd = DarkNet::RecieveFrom(m_socket->sd, pck.msg.buffer, pck.msg.capacity(), pck.address.addr);

			if (bytes_rcvd > 0)
			{
				m_data_callback(&pck, &(pck.address.addr));
			}			
		}

		return bytes_rcvd;
	}

	int DarkNetwork::Send(Packet* pck)
	{
		return DarkNet::SendTo(m_socket->sd, pck->msg.buffer, pck->msg.size(), pck->address.addr);
	}

	int DarkNetwork::Broadcast(Message msg)
	{
		Address broadcast_addr("255.255.255.255", DN_NETWORK_PORT_NUM);
		return DarkNet::Broadcast(m_socket->sd, msg.buffer, msg.size(), broadcast_addr.addr);
	}

	void DarkNetwork::SetBlockingMode(eBlockingMode mode)
	{
		DarkNet::SetBlockingMode(m_socket->sd, mode);
	}
}

