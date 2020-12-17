#include <stdexcept>
#include "Endpoint.hpp"

namespace cage
{
	namespace networking
	{
		Endpoint::Endpoint(unsigned short port) : m_local(true), m_address({0, 0})
		{
			m_socket = SDLNet_UDP_Open(port);
			m_sendPacket = SDLNet_AllocPacket(512);
			m_recvPacket = SDLNet_AllocPacket(512);
		}

		Endpoint::Endpoint(IPaddress address) : m_local(false), m_address(address), m_recvPacket(nullptr), m_sendPacket(nullptr)
		{

		}

		Endpoint::~Endpoint()
		{
			SDLNet_UDP_Close(m_socket);
		}

		void Endpoint::Send(char* dataBuffer, size_t size, const Endpoint& destination)
		{
			if (!IsLocal())
				throw std::runtime_error("Only local endpoints can send!");
			m_sendPacket->address = destination.GetIP();
			m_sendPacket->len = size;
			SDL_memcpy(m_sendPacket->data, dataBuffer, size);
			SDLNet_UDP_Send(m_socket, -1, m_sendPacket);
		}

		void Endpoint::Receive(void* dataBuffer, size_t& size)
		{
			if (!IsLocal())
				throw std::runtime_error("Only local endpoints can receive!");
			if (size = SDLNet_UDP_Recv(m_socket, m_recvPacket))
			{
				SDL_memcpy(dataBuffer, m_recvPacket->data, m_recvPacket->len);
				size = m_recvPacket->len;
			}
		}
	}
}