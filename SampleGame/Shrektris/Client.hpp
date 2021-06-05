#pragma once

#include "../../CAGE/IO/Networking/ClientConnection.hpp"

class Lobby;
class Gameplay;

class Client
{
	void processPacket(UDPpacket* packet);
	cage::networking::ClientConnection m_connection;
	int m_clientID;
	Lobby* m_lobby;
	Gameplay* m_gameplay;

public:

	Client(cage::networking::Endpoint&& server, Lobby* lobby);
	~Client();

	inline int GetID() { return m_clientID; }
	inline void SetGameplay(Gameplay* gameplay) { m_gameplay = gameplay; }

	void Listen();
	void Connect();
	void Send(void* data, size_t size);
	void UpdateTimeouts(float dt);

};