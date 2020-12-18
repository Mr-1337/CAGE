#pragma once

#include "../../CAGE/IO/Networking/ClientConnection.hpp"

class Client
{
	void processPacket(UDPpacket* packet)
	{

		using namespace cage::networking::packets;
		using namespace cage::networking;

		unsigned char* bytes = packet->data;
		switch ((PacketType)bytes[0])
		{
		case PacketType::QUERY_RESPONSE:
		{
			QueryResponse response;
			auto temp = name.substr(0, 15).c_str();
			SDL_memcpy(response.name, temp, 16);
			m_connection.Send(&response, sizeof(response), packet->address);
		}
		break;
		case PacketType::CONNECTION_ACCEPT:
		{
			m_connection.AddClient(std::move(Endpoint(packet->address)));
			m_clients[m_nextClient] = packet->address;
			m_nextClient++;
		}
		break;
		case PacketType::ROSTER_SYNC:
			break;
		case PacketType::GAME_START:
			break;
		case PacketType::BOARD_SYNC:
			break;
		}
	}

	cage::networking::ClientConnection m_connection;
	IPaddress m_clients[8];
	int m_nextClient;

public:

	Client(cage::networking::Endpoint server) : m_connection(server), m_nextClient(0)
	{
		std::cout << "Starting Shrektris client " << port << std::endl;
	}

	~Client()
	{
		std::cout << "Closing Shrektris client" << std::endl;
	}

};