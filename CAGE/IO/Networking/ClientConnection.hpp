#pragma once

#include <utility>
#include "Connection.hpp"

namespace cage
{
	namespace networking
	{
		class ClientConnection : public Connection
		{
			Endpoint m_server, m_local;
		public:
			// Opens a connection to the given server. This should only be used by clients.
			ClientConnection(Endpoint server) : m_local(0), m_server(std::move(server))
			{
				packets::Query query;
				Send(&query, sizeof(packets::Query));
			}

			bool Receive(UDPpacket* packet)
			{
				return m_local.Receive(packet);
			}
			void Send(void* dataBuffer, size_t size)
			{
				m_local.Send((char*)dataBuffer, size, m_server);
			}
		};
	}
}