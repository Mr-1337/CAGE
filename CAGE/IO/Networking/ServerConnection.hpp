#pragma once

#include <vector>
#include "Connection.hpp"

namespace cage
{
	namespace networking
	{
		class ServerConnection : Connection
		{
			std::vector<Endpoint> m_clients;
			Endpoint m_local;
		public:
			// Opens a server connection on the given port. Only should be used by servers.
			ServerConnection(int port) : m_local(port)
			{

			}
			virtual void Receive(void* dataBuffer, size_t size)
			{
				m_local.Receive(dataBuffer, size);
			}
			virtual void Send(void* dataBuffer, size_t size)
			{
				for (auto& e : m_clients)
				{
					m_local.Send((char*)dataBuffer, size, e);
				}
			}
			virtual void Send(void* dataBuffer, size_t size, size_t client)
			{
				m_local.Send((char*)dataBuffer, size, m_clients[client]);
			}
		};
	}
}