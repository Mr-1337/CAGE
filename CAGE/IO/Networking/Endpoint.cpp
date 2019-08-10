#include <exception>
#include "Endpoint.hpp"

namespace cage
{
	namespace networking
	{
		Endpoint::Endpoint(int port) : m_local(true)
		{
			m_socket = SDLNet_UDP_Open(port);
			m_sendPacket = SDLNet_AllocPacket(64);
			m_recvPacket = SDLNet_AllocPacket(64);
		}

		Endpoint::Endpoint(IPaddress address) : m_local(false), m_address(address)
		{

		}

		void Endpoint::Send(char* dataBuffer, size_t size, const Endpoint& destination)
		{
			if (!IsLocal())
				throw std::exception("Only local endpoints can send!");
			m_sendPacket->address = destination.GetIP();
			m_sendPacket->len = size;
			SDL_memcpy(m_sendPacket->data, dataBuffer, size);
			SDLNet_UDP_Send(m_socket, -1, m_sendPacket);
		}

		void Endpoint::Receive(void* dataBuffer, size_t& size)
		{
			if (!IsLocal())
				throw std::exception("Only local endpoints can receive!");
			if (SDLNet_UDP_Recv(m_socket, m_recvPacket))
			{
				dataBuffer = m_recvPacket->data;
				size = m_recvPacket->len;
			}
		}
	}
}