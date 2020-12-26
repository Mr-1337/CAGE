#include "Server.hpp"
#include "Lobby.hpp"
#include "Gameplay.hpp"

void Server::processPacket(UDPpacket* packet)
{
	bool val = false;
	using namespace cage::networking::packets;
	using namespace cage::networking;

	unsigned char* bytes = packet->data;

	std::cout << "got packets\n\n";

	switch ((PacketType)bytes[0])
	{
	case PacketType::QUERY:
	{
		QueryResponse response;
		std::string temp = name;
		SDL_memcpy(response.name, temp.c_str(), 16);
		m_serverSocket.Send((char*)&response, sizeof(response), packet->address);
	}
	break;
	case PacketType::CONNECTION_REQUEST:
	{
		addClient(Endpoint(packet->address));
		ConnectionAccept accept;
		accept.playerID = m_nextClient;
		m_connections[m_nextClient] = std::make_unique<ServerConnection>(&m_serverSocket, Endpoint(packet->address));
		m_connections[m_nextClient]->Send(&accept, sizeof(accept));

		RosterSync sync;
		SDL_memcpy(sync.lobbyName, name.c_str(), 16);
		sync.players[0] = true;
		for (int i = 0; i < m_connections.size(); i++)
			sync.players[i + 1] = m_connections[i] != nullptr;

		SendAll(&sync, sizeof(sync));

		m_lobby->acceptConnection(m_nextClient);
		m_nextClient++;
	}
	break;
	case PacketType::BOARD_SYNC:
	{
		BoardSync sync;
		SDL_memcpy(&sync, packet->data, sizeof(sync));
		m_gameplay->boardSync(sync);
		break;
	}
	case PacketType::PLAYER_PIECE_POS:
	{
		PlayerPiecePos pos;
		SDL_memcpy(&pos, packet->data, sizeof(pos));
		SendAll(&pos, sizeof(pos));
		m_gameplay->playerSync(pos);
		break;
	}
	}
}

Server::Server(unsigned int port, Lobby* lobby) : m_serverSocket(port), m_nextClient(1), m_lobby(lobby), m_gameplay(nullptr)
{
	std::cout << "Starting Shrektris game server on port " << port << std::endl;
}

Server::~Server()
{
	std::cout << "Closing Shrektris game server" << std::endl;
}

void Server::SendAll(void* data, size_t size)
{
	for (auto& connection : m_connections)
		if (connection)
		{
			connection->Send(data, size);
		}
}

void Server::addClient(cage::networking::Endpoint)
{

}

void Server::Listen()
{
	UDPpacket* packet = SDLNet_AllocPacket(2048);
	
	if(m_serverSocket.Receive(packet))
		processPacket(packet);

	SDLNet_FreePacket(packet);
}

void Server::UpdateTimeouts(float dt)
{
	for (auto& connection : m_connections)
		if (connection && !connection->AdvanceTimeout(dt))
		{
			std::cout << "Connection timed out" << std::endl;
		}
}
