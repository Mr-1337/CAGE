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
			}

			virtual void Receive(void* dataBuffer, size_t size) = 0;
			virtual void Send(void* dataBuffer, size_t size) = 0;
		};
	}
}