#pragma once

#include <iostream>
#include "Connection.hpp"

namespace cage
{
	namespace networking
	{
		class ServerConnection : public Connection
		{
			Endpoint *m_local, m_remote;
		public:
			// Opens a server connection on the given port. Only should be used by servers.
			ServerConnection(Endpoint* serverSocket, Endpoint&& remote) : m_local(serverSocket), m_remote(std::move(remote))
			{
				std::cout << "Server connection opened with " << m_remote.GetIPAsString() << std::endl;
			}

			~ServerConnection()
			{
				std::cout << "Server connection closed" << std::endl;
			}

			virtual bool Receive(UDPpacket* packet)
			{
				return m_local->Receive(packet);
			}

			virtual void Send(void* dataBuffer, size_t size)
			{
				m_local->Send((char*)dataBuffer, size, m_remote);
			}

			void Send(void* dataBuffer, size_t size, IPaddress ip)
			{
				m_local->Send((char*)dataBuffer, size, ip);
			}
		};
	}
}