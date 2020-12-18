#pragma once

#include <utility>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../../CAGE/Core/GameState.hpp"
#include "../../CAGE/Graphics/UI/LayoutGroup.hpp"
#include "../../CAGE/IO/Networking/ClientConnection.hpp"
#include "../../CAGE/IO/Networking/ServerConnection.hpp"
#include "../../CAGE/Graphics/UI/TextField.hpp"

#include "../MenuButton.hpp"
#include "Server.hpp"

class Lobby : public cage::GameState
{
public:

	Lobby(cage::Game& game);

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
	std::shared_ptr<cage::ui::LayoutGroup> m_hostPanel, m_connectPanel, m_lobbyPanel;
	cage::Font m_font;

	void makeHostPanel();
	void makeConnectPanel();
	void makeLobbyPanel();

	enum Mode
	{
		MAIN,
		HOST,
		JOIN
	};

	Mode m_mode;
	std::unique_ptr<cage::networking::ClientConnection> m_clientConnection;
	std::unique_ptr<Server> m_server;

	void acceptConnection(const std::string& name);

};