#pragma once

#include <string>
#include "../../CAGE/IO/Networking/ServerConnection.hpp"

class Server
{
	void processPacket(UDPpacket* packet)
	{

		using namespace cage::networking::packets;
		using namespace cage::networking;

		unsigned char* bytes = packet->data;
		switch ((PacketType)bytes[0])
		{
		case PacketType::QUERY:
		{
			QueryResponse response;
			auto temp = name.substr(0, 15).c_str();
			SDL_memcpy(response.name, temp, 16);
			m_connection.Send(&response, sizeof(response), packet->address);
		}
			break;
		case PacketType::CONNECTION_REQUEST:
		{
			m_connection.AddClient(std::move(Endpoint(packet->address)));
			ConnectionAccept accept;
			m_connection.Send(&accept, sizeof(accept), m_nextClient);
			m_nextClient++;
		}
			break;
		}
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

	void Listen()
	{
		UDPpacket* packet = SDLNet_AllocPacket(2048);
		m_connection.Receive(packet);
		processPacket(packet);
		SDLNet_FreePacket(packet);
	}

	std::string name;

};