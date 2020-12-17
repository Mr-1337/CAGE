#pragma once

#include "Connection.hpp"

namespace cage
{
	namespace networking
	{
		class ClientConnection : Connection
		{
			Endpoint m_server, m_local;
		public:
			// Opens a connection to the given server. This should only be used by clients.
			ClientConnection(Endpoint server) : m_local(0), m_server(server)
			{
				char payload[6] = "Shrek";
				Send(payload, 6);
			}

			void Receive(void* dataBuffer, size_t size)
			{

			}
			void Send(void* dataBuffer, size_t size)
			{
				m_local.Send((char*)dataBuffer, size, m_server);
			}
		};
	}
}