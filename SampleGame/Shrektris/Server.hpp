#pragma once

#include <string>
#include "../../CAGE/IO/Networking/ServerConnection.hpp"

class Server
{
	bool processPacket(UDPpacket* packet)
	{
		bool val = false;
		using namespace cage::networking::packets;
		using namespace cage::networking;

		unsigned char* bytes = packet->data;
		switch ((PacketType)bytes[0])
		{
		case PacketType::QUERY:
		{
			QueryResponse response;
			std::string temp = name;
			SDL_memcpy(response.name, temp.c_str(), 16);
			m_connection.Send(&response, sizeof(response), packet->address);
		}
			break;
		case PacketType::CONNECTION_REQUEST:
		{
			m_connection.AddClient(std::move(Endpoint(packet->address)));
			ConnectionAccept accept;
			m_connection.Send(&accept, sizeof(accept), m_nextClient);
			m_nextClient++;
			val = true;
		}
			break;
		}
		return val;
	}

	cage::networking::ServerConnection m_connection;
	int m_nextClient;

public:

	Server(unsigned int port) : m_connection(port), m_nextClient(0)
	{
		std::cout << "Starting Shrektris game server on port " << port << std::endl;
	}

	~Server()
	{
		std::cout << "Closing Shrektris game server" << std::endl;
	}

	bool Listen()
	{
		UDPpacket* packet = SDLNet_AllocPacket(2048);
		m_connection.Receive(packet);
		bool res = processPacket(packet);
		SDLNet_FreePacket(packet);
		return res;
	}

	std::string name;

};