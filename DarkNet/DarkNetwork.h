#ifndef DARK_NETWORK_H
#define DARK_NETWORK_H
#include "DarkNetFuncs.h"
#include "DarkNetStructs.h"
#include <vector>

namespace DarkNet
{
	struct UDPSocket;
	struct Address;
	struct Buffer;
	struct Packet;

	typedef void(*OnPeerFound)			( void* params	);
	typedef void(*OnDataRecieved)		( Packet *pck	);	

	class DarkNetwork
	{		
	public:
		static DarkNetwork& Instance()
		{
			if (m_pInstance == nullptr)
			{
				m_pInstance = new DarkNetwork();
			}
			return *m_pInstance;
		}
		~DarkNetwork();
		
		void	Start(OnPeerFound connection_callback, OnDataRecieved data_callback);
		void	Stop();
		int		Send(Packet &pck);
		void	Update();
		int		Broadcast(Buffer &msg);
		void	SetBlockingMode(DarkNet::eBlockingMode mode);
		void	SendAll(Buffer buff);
		void	Host(size_t maxClients);
		Peer*	GetPeer(Address &addr);

	private:
		DarkNetwork();
		static DarkNetwork* m_pInstance;
		void				Listen();
		void				WriteNetworkData();
		int					ReadNetworkData();
		void				ProcessReadPackets(Packet &pck);
		void				CheckHostMsgs(Packet &pck);
		void				CheckClientMsgs(Packet &pck);
		bool				TryDispatchPackets(Packet &pck);
		void				AddNewPeer(Packet &pck, eMode mode);		

		size_t				m_maxClients;
		eListeningState		m_runningState;
		eMode				m_mode;
		UDPSocket*			m_pSocket;
		Address*			m_pAddress;
		OnPeerFound			m_newPeerCallback;
		OnDataRecieved		m_DataCallback;
		std::vector<Peer>	m_peers;
	};
}

#endif