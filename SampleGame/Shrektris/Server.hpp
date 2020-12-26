#pragma once

#include <string>
#include <memory>
#include <array>
#include "../../CAGE/IO/Networking/ServerConnection.hpp"
class Lobby;
class Gameplay;

class Server
{
	void processPacket(UDPpacket* packet);
	std::array<std::unique_ptr<cage::networking::ServerConnection>, 7> m_connections;
	int m_nextClient;
	Lobby* m_lobby;
	Gameplay* m_gameplay;
	cage::networking::Endpoint m_serverSocket;

	void addClient(cage::networking::Endpoint client);

public:

	Server(unsigned int port, Lobby* lobby);
	~Server();

	inline void SetGameplay(Gameplay* gameplay) { m_gameplay = gameplay; }

	void SendAll(void* data, size_t size);
	void Listen();
	void UpdateTimeouts(float dt);
	std::string name;

};