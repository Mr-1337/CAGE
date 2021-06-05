#include "Lobby.hpp"
#include "../../CAGE/Core/Game.hpp"
#include "../../CAGE/Graphics/UI/FlowLayout.hpp"
#include "../../CAGE/Graphics/UI/GridLayout.hpp"
#include "Gameplay.hpp"

Lobby::Lobby(cage::Game& game) : cage::GameState(game), m_font("Assets/sans.ttf", 32)
{
	std::shared_ptr<MenuButton> host = std::make_shared<MenuButton>("Host");
	std::shared_ptr<MenuButton> join = std::make_shared<MenuButton>("Join");

	m_buttonGroup = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 0.f, 20.f }, cage::ui::FlowLayout::Orientation::VERTICAL, false));
	m_buttonGroup->Add(host);
	m_buttonGroup->Add(join);

	m_mode = MAIN;

	host->OnClick = [this]()
	{
		if (m_mode == MAIN)
		{
			m_mode = HOST;
			cage::ui::transforms::Move t({ -m_root.GetSize().x / 2.f - m_hostPanel->GetSize().x / 2.0f, 0.f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
			m_buttonGroup->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
			m_hostPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		}
	};

	join->OnClick = [this]()
	{
		if (m_mode == MAIN)
		{
			m_mode = JOIN;
			cage::ui::transforms::Move t({ -m_root.GetSize().x / 2.f - m_connectPanel->GetSize().x / 2.0f, 0.f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
			m_buttonGroup->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
			m_connectPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		}
	};
	
	makeConnectPanel();
	makeHostPanel();
	makeLobbyPanel();

	auto size = getGame().GetWindow().GetSize();
	m_spriteShader = cage::ui::UIElement::shader;
	m_root.Resize({ size.first, size.second });
	m_root.SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);

	m_root.Add(m_buttonGroup);

	m_root.Add(m_connectPanel);
	m_root.Add(m_hostPanel);

	m_input.Subscribe(host.get());
	m_input.Subscribe(join.get());
	m_startSound = Mix_LoadWAV("Assets/start.ogg");
}

Lobby::~Lobby()
{
	Mix_FreeChunk(m_startSound);
}

void Lobby::makeHostPanel()
{
	m_hostPanel = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 10.f, 20.f }, cage::ui::FlowLayout::Orientation::VERTICAL, false));
	m_hostPanel->SetParentMounting(cage::ui::MountPoint::CENTER_RIGHT);
	m_hostPanel->SetLocalMounting(cage::ui::MountPoint::CENTER_LEFT);
	m_lobbyField = std::make_shared<cage::ui::TextField>(m_font, 15);	

	auto label = std::make_shared<cage::ui::Text>(m_font);
	label->SetText("Lobby Name: ");
	auto textGroup = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 10.f, 0.f }, cage::ui::FlowLayout::Orientation::HORIZONTAL, true));
	auto hostB = std::make_shared<MenuButton>("Open Lobby");
	hostB->OnClick = [this]()
	{
		if (m_mode == HOST)
		{
			m_mode = LOBBY;
			hostLobby(m_lobbyField->GetText());
		}
	};

	auto backB = std::make_shared<MenuButton>("Back");
	backB->OnClick = [this]()
	{
		m_mode = MAIN;
		cage::ui::transforms::Move t({ m_root.GetSize().x / 2.f + m_hostPanel->GetSize().x / 2.0f, 0.f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
		m_buttonGroup->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_hostPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_server = nullptr;
	};

	textGroup->Add(label);
	textGroup->Add(m_lobbyField);
	m_hostPanel->Add(textGroup);
	m_hostPanel->Add(hostB);
	m_hostPanel->Add(backB);

	m_input.Subscribe(hostB.get());
	m_input.Subscribe(backB.get());
	m_input.Subscribe(m_lobbyField.get());
}

void Lobby::makeConnectPanel()
{
	m_connectPanel = std::make_shared <cage::ui::LayoutGroup>(new cage::ui::GridLayout({ 10.f, 20.f }, 3));
	auto preText = std::make_shared<cage::ui::Text>(m_font);
	m_ipTextField = std::make_shared<cage::ui::TextField>(m_font, 15);
	auto search = std::make_shared<MenuButton>("Connect");
	auto backC = std::make_shared<MenuButton>("Back");
	search->OnClick = [this]()
	{
		//acceptConnection(m_ipTextField->GetText());
		IPaddress ip;
		SDLNet_ResolveHost(&ip, m_ipTextField->GetText().c_str(), 25570);
		m_client = std::make_unique<Client>(cage::networking::Endpoint(ip), this);
	};
	backC->OnClick = [this]()
	{
		m_mode = MAIN;
		cage::ui::transforms::Move t({ m_root.GetSize().x / 2.f + m_connectPanel->GetSize().x / 2.0f, 0.f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
		m_buttonGroup->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_connectPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_client = nullptr;
	};
	search->Scale(0.4);
	backC->Scale(0.4);

	preText->SetText("Enter IP: ");
	m_connectPanel->Add(preText);
	m_connectPanel->Add(m_ipTextField);
	m_connectPanel->Add(search);
	m_connectPanel->Add(backC);
	m_ipTextField->SetColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	m_connectPanel->SetParentMounting(cage::ui::MountPoint::CENTER_RIGHT);
	m_connectPanel->SetLocalMounting(cage::ui::MountPoint::CENTER_LEFT);

	m_input.Subscribe(search.get());
	m_input.Subscribe(backC.get());
	m_input.Subscribe(m_ipTextField.get());
}

void Lobby::makeLobbyPanel()
{
	m_lobbyPanel = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::GridLayout({ 30.f, 0.f }, 2));
	auto left = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::GridLayout({ 0.f, 30.f }, 1));
	m_lobbyPanel->SetParentMounting(cage::ui::MountPoint::TOP);
	m_lobbyPanel->SetLocalMounting(cage::ui::MountPoint::BOTTOM);

	m_lobbyNameText = std::make_shared<cage::ui::Text>(m_font);
	m_lobbyNameText->SetText("Unspecified Lobby Name");

	m_playerCountText = std::make_shared<cage::ui::Text>(m_font);
	m_playerCountText->SetText("unspecified");

	auto text2 = std::make_shared<cage::ui::Text>(m_font);
	text2->SetText("unknown game mode");

	auto start = std::make_shared<MenuButton>("START THE GAME ALREADY!");
	start->Scale(0.4);

	start->OnClick = [this]()
	{
		startGame();
	};

	left->Add(m_lobbyNameText);
	left->Add(m_playerCountText);
	left->Add(text2);
	left->Add(start);

	m_playerList = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 0.f, 0.f }, cage::ui::FlowLayout::Orientation::VERTICAL, false));
	m_lobbyPanel->Add(left);
	m_lobbyPanel->Add(m_playerList);

	m_root.Add(m_lobbyPanel);
	m_input.Subscribe(start.get());

}

void Lobby::hostLobby(const std::string& lobbyName)
{
	m_playerCount = 1;
	std::string name;
	name = lobbyName;
	if (name.empty())
		name = "Unspecified Lobby Name";

	m_server = std::make_unique<Server>(25570, this);
	m_server->name = name;
	m_lobbyNameText->SetText(name);
	m_playerCountText->SetText(std::to_string(1) + "/8 Players");
	addPlayer(0);
	m_lobbyPanel->Compress();

	cage::ui::transforms::Move t({ 0.0, m_root.GetSize().y / 2.0f + m_lobbyPanel->GetSize().y / 2.0f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
	m_lobbyPanel->ScheduleTransform(std::make_unique<cage::ui::transforms::Move>(t));
	m_hostPanel->ScheduleTransform(std::make_unique<cage::ui::transforms::Move>(t));

}

void Lobby::joinLobby()
{
	cage::ui::transforms::Move t({ 0.0, m_root.GetSize().y / 2.f + m_lobbyPanel->GetSize().y / 2.0f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
	m_lobbyPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
	m_connectPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
}

void Lobby::addPlayer(int playerID)
{
	using namespace std;
	using namespace cage::ui;

	auto text = make_shared<Text>(m_font);
	Player p(playerID);
	text->SetColor({ p.GetColor(), 1.0f });
	text->SetText("Player " + std::to_string(playerID + 1));
	m_playerList->Add(text);
}

void Lobby::acceptConnection(int clientID)
{
	std::cout << "Player " << clientID << " connected" << std::endl;
	m_playerCount++;
	m_playerCountText->SetText(std::to_string(m_playerCount) + "/8 Players");
	addPlayer(clientID);
}

void Lobby::syncRoster(cage::networking::packets::RosterSync sync)
{
	m_lobbyNameText->SetText(sync.lobbyName);
	int pcount = 0;
	for (int i = 0; i < 8; i++)
		if (sync.players[i])
			pcount++;
	m_playerCount = pcount;
	m_playerCountText->SetText(std::to_string(m_playerCount) + "/8 Players");
}

void Lobby::startGame()
{
	using namespace cage::networking::packets;
	if (m_server)
	{
		GameStart startSignal;
		startSignal.boardH = 20;
		startSignal.boardW = m_playerCount * 10;
		startSignal.playerCount = m_playerCount;
		m_server->SendAll(&startSignal, sizeof(startSignal));
		s_stateMachine->Push(new Gameplay(getGame(), startSignal, std::move(m_server)));
	}
	else
	{
		Mix_PlayChannel(1, m_startSound, 0);
	}
}

void Lobby::startGameClient(cage::networking::packets::GameStart start)
{
	s_stateMachine->Push(new Gameplay(getGame(), start, std::move(m_client)));
}

void Lobby::showLobby(const std::string& lobbyName)
{
	if (m_joinButton)
	{
		m_connectPanel->Remove(m_joinButton);
		m_input.Unsubscribe(m_joinButton.get());
	}
	m_joinButton = std::make_shared<MenuButton>(std::string("Join lobby ") + lobbyName);
	m_joinButton->Scale(0.4);
	m_input.Subscribe(m_joinButton.get());
	m_connectPanel->Add(m_joinButton);
}

void Lobby::OnRevealed()
{
}

void Lobby::OnHidden()
{

}

void Lobby::ProcessEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		m_input.Raise(e);
		switch (e.type)
		{
		case SDL_QUIT:
			quit();
			break;
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				glViewport(0, 0, e.window.data1, e.window.data2);
				m_root.Resize({ e.window.data1, e.window.data2 });
				m_spriteShader->Use();
				m_spriteShader->Projection->value = glm::ortho(0.f, (float)e.window.data1, (float)e.window.data2, 0.f);
				m_spriteShader->Projection->ForwardToShader();
			}
			break;
		}
	}
}
void Lobby::Update(float delta)
{
	m_root.Update(delta);
	if (m_server)
	{
		m_server->Listen();
	}
	else if (m_client)
	{
		m_client->Listen();
	}
}
void Lobby::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	m_spriteShader->Use();
	m_root.Draw();
}