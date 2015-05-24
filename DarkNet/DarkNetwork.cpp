#include "DarkNetwork.h"
#include "DarkNetFuncs.h"
#include "Log.h"


namespace DarkNet
{
	DarkNetwork* DarkNetwork::m_pInstance = nullptr;
	
	DarkNetwork::DarkNetwork()
	{
		m_runningState = eInactive;
		m_mode = eIdle;
	}


	DarkNetwork::~DarkNetwork()
	{

	}

	void DarkNetwork::Start(OnPeerFound connection_callback, OnDataRecieved data_callback)
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

		m_pSocket = new UDPSocket();
		m_pAddress = new Address(DN_NETWORK_PORT_NUM);
		int ret = DarkNet::Bind(m_pSocket->sd, m_pAddress->addr);
		if (ret != 0)
		{
			OUTPUT("Bind failed %d. Cannot listen for incoming packets", ret);
		}
		DarkNet::SetBlockingMode(m_pSocket->sd, DarkNet::eNonBlocking);

		m_newPeerCallback = connection_callback;
		m_DataCallback = data_callback;

		m_runningState = eActive;
	}

	void DarkNetwork::Stop()
	{
		DestroyNetwork();
		delete m_pSocket;
		delete m_pAddress;

		m_newPeerCallback	= nullptr;
		m_DataCallback		= nullptr;
	}

	void DarkNetwork::Update()
	{
		switch (m_runningState)
		{
		case eActive:
			ReadNetworkData();
			WriteNetworkData();
			break;
		case eInactive:
			break;
		}
	}

	void DarkNetwork::Listen()
	{

	}

	void DarkNetwork::WriteNetworkData()
	{
		for (size_t i = 0, size = m_peers.size(); i < size; ++i)
		{
			if (m_peers[i].output_msg.size())
			{
				Packet pck(m_peers[i].output_msg, m_peers[i].address);
				Send(pck);
				m_peers[i].output_msg.clear();
			}
			
		}
	}

	int DarkNetwork::ReadNetworkData()
	{
		Packet pck;
		int bytes_rcvd = 0;
		while (bytes_rcvd != -1)
		{
			bytes_rcvd = DarkNet::RecieveFrom(m_pSocket->sd, pck.buff.buffer, pck.buff.capacity(), pck.address.addr);

			if (bytes_rcvd > 0)
			{
				ProcessReadPackets(pck);

				m_DataCallback(&pck);
			}
		}

		return bytes_rcvd;
	}

	int DarkNetwork::Send(Packet &pck)
	{
		return DarkNet::SendTo(m_pSocket->sd, pck.buff.buffer, pck.buff.size(), pck.address.addr);
	}

	void DarkNetwork::SendAll(Buffer buff)
	{
		for (size_t i = 0, size = m_peers.size(); i < size; ++i)
		{
			Packet pck(buff, m_peers[i].address);
			Send(pck);
		}
	}

	int DarkNetwork::Broadcast(Buffer &msg)
	{
		Address broadcast_addr(DN_ADDRESS_BROADCAST, DN_NETWORK_PORT_NUM);
		return DarkNet::Broadcast(m_pSocket->sd, msg.buffer, msg.size(), broadcast_addr.addr);
	}

	void DarkNetwork::SetBlockingMode(eBlockingMode mode)
	{
		DarkNet::SetBlockingMode(m_pSocket->sd, mode);
	}

	void DarkNetwork::Host(size_t maxClients)
	{
		if (m_runningState == eInactive)
		{
			OUTPUT("Currently In Inactive state, cant host");
			return;
		}
		m_maxClients = maxClients;
		m_mode = eHost;
		m_peers.clear();
		m_peers.reserve(m_maxClients);
	}

	void DarkNetwork::ProcessReadPackets(Packet &pck)
	{		
		//Try dispatching packet to an existing peer, if succeeds need no go further
		if (TryDispatchPackets(pck))
		{
			return;
		}
		
		//else we check for special host/client msgs
		switch (m_mode)
		{
		case eHost:
			CheckHostMsgs(pck);
			break;
		case eClient:
			CheckClientMsgs(pck);
			break;
		default:
			break;
		}
	}

	bool DarkNetwork::TryDispatchPackets(Packet &pck)
	{
		bool packet_dispatched = false;
		Peer *peer = GetPeer(pck.address);
		if (peer)
		{
			peer->SetInputBuffer(pck.buff);
			packet_dispatched = true;
		}

		return packet_dispatched;
	}


	//Here we check for special msgs that relate to host
	void DarkNetwork::CheckHostMsgs(Packet &pck)
	{
		//Check for new connection
		if (pck.buff == DN_MSG_REQUEST_CONNECTION)
		{
			Packet ack = pck;
			//ToDo : Banned users?
			if (m_peers.size() < m_maxClients)
			{				
				//Notify all of a new peer
				Buffer ip;
				DarkNet::GetIp( &(pck.address.addr), ip.buffer, ip.capacity() );

				Buffer accept_msg(DN_MSG_CONNECTION_ACCEPTED);

				Buffer ack_msg = ip + Buffer("|") + accept_msg;

				SendAll(ack_msg);

				//Now add a new peer
				AddNewPeer(pck, eClient);
				ack.buff = DN_MSG_CONNECTION_ACCEPTED;
				Send(ack);
			}
			else
			{
				ack.buff = DN_MSG_SERVER_FULL;
				Send(ack);
			}
		}
	}

	//Here we check for special msgs that relate to client
	void DarkNetwork::CheckClientMsgs(Packet &pck)
	{		
		if (pck.buff == DN_MSG_CONNECTION_ACCEPTED)
		{
			//The one who sent this pck is host
			AddNewPeer(pck, eHost);
		}
		else if (pck.buff == DN_MSG_NEW_PEER_FORMED)
		{

		}		
	}

	void DarkNetwork::AddNewPeer(Packet &pck, eMode mode)
	{
		Peer new_peer;
		new_peer.address = pck.address;
		new_peer.mode = mode;
		m_peers.push_back(new_peer);

		m_newPeerCallback( &(m_peers.back()) );
	}

	Peer* DarkNetwork::GetPeer(Address &addr)
	{
		Peer* cur_peer = nullptr;
		Buffer from_ip, peer_ip;		
		DarkNet::GetIp(&addr.addr, from_ip.buffer, from_ip.capacity());

		for (int i = 0, size = m_peers.size(); i < size; ++i)
		{			
			cur_peer = &m_peers[i];
			DarkNet::GetIp(&(cur_peer->address.addr), peer_ip.buffer, peer_ip.capacity());
			
			if (strcmp(from_ip.buffer, peer_ip.buffer) == 0)
			{
				break;
			}
		}

		return cur_peer;
	}
}

