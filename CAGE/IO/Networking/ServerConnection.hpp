#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
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
			ServerConnection(unsigned short port) : m_local(port)
			{
				std::cout << "Server connection opened on port " << port << std::endl;
			}

			~ServerConnection()
			{
				std::cout << "Server connection closed" << std::endl;
			}

			void AddClient(Endpoint client)
			{
				//if (std::find(m_clients.begin(), m_clients.end(), client) == m_clients.end())
				{
					m_clients.emplace_back(std::move(client));
				}
			}

			virtual void Receive(UDPpacket* packet)
			{
				m_local.Receive(packet);
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

			void Send(void* dataBuffer, size_t size, IPaddress ip)
			{
				m_local.Send((char*)dataBuffer, size, ip);
			}
		};
	}
}