#include <stdexcept>
#include <iostream>
#include "Endpoint.hpp"

namespace cage
{
	namespace networking
	{
		Endpoint::Endpoint(unsigned short port) : m_local(true), m_address({0, 0})
		{
			m_socket = SDLNet_UDP_Open(port);
			m_sendPacket = SDLNet_AllocPacket(2048);
			m_recvPacket = SDLNet_AllocPacket(2048);
			m_dirty = false;
			std::cout << "Opened socket locally on port " << port << std::endl;
		}

		Endpoint::Endpoint(IPaddress address) : m_local(false), m_address(address), m_recvPacket(nullptr), m_sendPacket(nullptr), m_dirty(false)
		{
		}

		Endpoint::Endpoint(Endpoint&& other) noexcept
		{
			m_local = other.m_local;
			m_address = other.m_address;
			m_recvPacket = other.m_recvPacket;
			m_sendPacket = other.m_sendPacket;
			m_socket = other.m_socket;
			other.m_dirty = true;
			m_dirty = false;
		}

		Endpoint::~Endpoint()
		{
			if (!m_dirty && m_local)
			{
				std::cout << "Closed socket" << std::endl;
				SDLNet_UDP_Close(m_socket);
			}
		}

		std::string Endpoint::GetIPAsString() const
		{
			std::string ip;
			Uint32 host = GetIP().host;
			host = SDLNet_Read32(&host);
			Uint16 port = GetIP().port;

			for (int i = 3; i >= 0; i--)
			{
				Uint8 byte = (host >> (i * 8)) & 0xFF;
				ip += std::to_string(byte);
				ip += ".";
			}

			ip.pop_back();
			return ip;
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

		bool Endpoint::Receive(UDPpacket* packet)
		{
			if (!IsLocal())
				throw std::runtime_error("Only local endpoints can receive!");
			if (SDLNet_UDP_Recv(m_socket, m_recvPacket))
			{
				SDL_memcpy(packet->data, m_recvPacket->data, m_recvPacket->len);
				packet->len = m_recvPacket->len;
				packet->address = m_recvPacket->address;
				return true;
			}
			return false;
		}
	}
}