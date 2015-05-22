#ifndef DARK_NETWORK_H
#define DARK_NETWORK_H
#include "DarkNetFuncs.h"
#include "DarkNetStructs.h"

namespace DarkNet
{
	struct UDPSocket;
	struct Address;
	struct Message;
	struct Packet;

	typedef void(*OnPeerFound)			( void* params				);
	typedef void(*OnDataRecieved)		( Packet *pck, void* tag	);

	class DarkNetwork
	{
	public:
		static DarkNetwork& Instance()
		{
			if (m_instance == nullptr)
			{
				m_instance = new DarkNetwork();
			}
			return *m_instance;
		}
		~DarkNetwork();
		
		void	Start(OnPeerFound connection_callback, OnDataRecieved data_callback);
		void	Stop();
		int		Send(Packet *pck);
		void	Update();
		int		Broadcast(Message msg);
		void	SetBlockingMode(DarkNet::eBlockingMode mode);

	private:
		DarkNetwork();
		static DarkNetwork* m_instance;
		int					Listen();

		int					m_maxClients;
		UDPSocket			*m_socket;
		Address				*m_address;
		OnPeerFound			m_new_peer_callback;
		OnDataRecieved		m_data_callback;
	};
}

#endif