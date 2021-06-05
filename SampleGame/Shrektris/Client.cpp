#include "Client.hpp"
#include "Lobby.hpp"
#include "Gameplay.hpp"

void Client::processPacket(UDPpacket* packet)
{

	using namespace cage::networking::packets;
	using namespace cage::networking;

	unsigned char* bytes = packet->data;

	std::cout << "got packets\n\n";

	m_connection.ResetTimeout();

	switch ((PacketType)bytes[0])
	{
	case PacketType::QUERY_RESPONSE:
	{
		QueryResponse response;
		SDL_memcpy(&response, packet->data, sizeof(QueryResponse));
		m_lobby->showLobby(response.name);
		m_lobby->m_joinButton->OnClick = [this]()
		{
			ConnectionRequest req;
			m_connection.Send(&req, sizeof(req));
		};
	}
	break;
	case PacketType::CONNECTION_ACCEPT:
	{
		ConnectionAccept accept;
		SDL_memcpy(&accept, packet->data, sizeof(accept));
		m_clientID = accept.playerID;
		m_lobby->joinLobby();
	}
	break;
	case PacketType::ROSTER_SYNC:
	{
		RosterSync sync;
		SDL_memcpy(&sync, packet->data, sizeof(sync));
		m_lobby->syncRoster(sync);
	}
		break;
	case PacketType::GAME_START:
	{
		GameStart start;
		SDL_memcpy(&start, packet->data, sizeof(start));
		m_lobby->startGameClient(start);
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
		m_gameplay->playerSync(pos);
		break;
	}
	}
}

void Client::Listen()
{
	UDPpacket* packet = SDLNet_AllocPacket(2048);
	if(m_connection.Receive(packet))
		processPacket(packet);
	SDLNet_FreePacket(packet);
}

void Client::Connect()
{
	cage::networking::packets::ConnectionRequest request;
	m_connection.Send(&request, sizeof(request));
}

void Client::Send(void* data, size_t size)
{
	m_connection.Send(data, size);
}

void Client::UpdateTimeouts(float dt)
{
	if (!m_connection.AdvanceTimeout(dt))
	{
		std::cout << "Connection timed out" << std::endl;
	}
}

Client::Client(cage::networking::Endpoint&& server, Lobby* lobby) : m_connection(std::move(server)), m_clientID(-1), m_lobby(lobby), m_gameplay(nullptr)
{
	std::cout << "Starting Shrektris client " << std::endl;
}

Client::~Client()
{
	std::cout << "Closing Shrektris client" << std::endl;
}
