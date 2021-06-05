#pragma once

#include <utility>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_mixer.h>
#include "../../CAGE/Core/GameState.hpp"
#include "../../CAGE/Graphics/UI/LayoutGroup.hpp"
#include "../../CAGE/IO/Networking/ClientConnection.hpp"
#include "../../CAGE/IO/Networking/ServerConnection.hpp"
#include "../../CAGE/Graphics/UI/TextField.hpp"

#include "../MenuButton.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Player.hpp"

class Lobby : public cage::GameState
{
public:

	Lobby(cage::Game& game);
	~Lobby();

	void OnRevealed() override;
	void OnHidden() override;

	void ProcessEvents() override;
	void Update(float delta) override;
	void Draw() override;

private:
	cage::ui::UIElement m_root;
	std::shared_ptr<cage::SpriteShader> m_spriteShader;
	std::shared_ptr<cage::ui::LayoutGroup> m_buttonGroup;
	std::shared_ptr<cage::ui::TextField> m_ipTextField;
	std::shared_ptr<cage::ui::TextField> m_lobbyField;
	std::shared_ptr<cage::ui::Text> m_lobbyNameText, m_playerCountText;
	std::shared_ptr<cage::ui::LayoutGroup> m_hostPanel, m_connectPanel, m_lobbyPanel, m_playerList;
	std::shared_ptr<MenuButton> m_joinButton;
	cage::Font m_font;

	void makeHostPanel();
	void makeConnectPanel();
	void makeLobbyPanel();

	void addPlayer(int playerID);
	

	void startGame();
	void startGameClient(cage::networking::packets::GameStart start);
	void hostLobby(const std::string& lobbyName);
	void joinLobby();
	void showLobby(const std::string& lobbyName);
	void syncRoster(cage::networking::packets::RosterSync sync);

	int m_playerCount;

	enum Mode
	{
		MAIN,
		HOST,
		JOIN,
		LOBBY
	};

	Mode m_mode;
	Mix_Chunk* m_startSound;
	std::unique_ptr<Server> m_server;
	std::unique_ptr<Client> m_client;

	void acceptConnection(int clientID);

	friend class Server;
	friend class Client;

};