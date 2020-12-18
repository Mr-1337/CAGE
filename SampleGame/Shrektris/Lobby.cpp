#include "Lobby.hpp"
#include "../../CAGE/Core/Game.hpp"
#include "../../CAGE/Graphics/UI/FlowLayout.hpp"
#include "../../CAGE/Graphics/UI/GridLayout.hpp"

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
			m_server = std::make_unique<Server>(25570);
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
	m_root.Add(m_lobbyPanel);

	m_input.Subscribe(host.get());
	m_input.Subscribe(join.get());

}

void Lobby::makeHostPanel()
{
	m_hostPanel = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 10.f, 20.f }, cage::ui::FlowLayout::Orientation::VERTICAL, false));
	m_hostPanel->SetParentMounting(cage::ui::MountPoint::CENTER_RIGHT);
	m_hostPanel->SetLocalMounting(cage::ui::MountPoint::CENTER_LEFT);
	m_lobbyField = std::make_shared<cage::ui::TextField>(m_font, 15);

	auto backH = std::make_shared<MenuButton>("Back");
	backH->OnClick = [this]()
	{
		m_mode = MAIN;
		cage::ui::transforms::Move t({ m_root.GetSize().x / 2.f + m_hostPanel->GetSize().x / 2.0f, 0.f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
		m_buttonGroup->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_hostPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_server = nullptr;
	};

	m_hostPanel->Add(m_lobbyField);
	m_hostPanel->Add(backH);
	m_input.Subscribe(backH.get());
	m_input.Subscribe(m_lobbyField.get());
}

void Lobby::makeConnectPanel()
{
	m_connectPanel = std::make_shared <cage::ui::LayoutGroup>(new cage::ui::GridLayout({ 10.f, 20.f }, 3));
	auto preText = std::make_shared<cage::ui::Text>(m_font);
	m_ipTextField = std::make_shared<cage::ui::TextField>(m_font, 15);
	auto connect = std::make_shared<MenuButton>("Connect");
	auto backC = std::make_shared<MenuButton>("Back");
	connect->OnClick = [this]()
	{
		//acceptConnection(m_ipTextField->GetText());
		IPaddress ip;
		SDLNet_ResolveHost(&ip, m_ipTextField->GetText().c_str(), 25570);
		m_clientConnection = std::make_unique<cage::networking::ClientConnection>(cage::networking::Endpoint(ip));
	};
	backC->OnClick = [this]()
	{
		m_mode = MAIN;
		cage::ui::transforms::Move t({ m_root.GetSize().x / 2.f + m_connectPanel->GetSize().x / 2.0f, 0.f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
		m_buttonGroup->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_connectPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_clientConnection = nullptr;
	};
	connect->Scale(0.4);
	backC->Scale(0.4);

	preText->SetText("Enter IP: ");
	m_connectPanel->Add(preText);
	m_connectPanel->Add(m_ipTextField);
	m_connectPanel->Add(connect);
	m_connectPanel->Add(backC);
	m_ipTextField->SetColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	m_connectPanel->SetParentMounting(cage::ui::MountPoint::CENTER_RIGHT);
	m_connectPanel->SetLocalMounting(cage::ui::MountPoint::CENTER_LEFT);

	m_input.Subscribe(connect.get());
	m_input.Subscribe(backC.get());
}

void Lobby::makeLobbyPanel()
{
	m_lobbyPanel = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::GridLayout({ 0.0f, 30.f }, 1));
	m_lobbyPanel->SetParentMounting(cage::ui::MountPoint::TOP);
	m_lobbyPanel->SetLocalMounting(cage::ui::MountPoint::BOTTOM);

	auto text = std::make_shared<cage::ui::Text>(m_font);
	text->SetText("unspecified");

	auto text2 = std::make_shared<cage::ui::Text>(m_font);
	text2->SetText("unknown amount of players");

	auto start = std::make_shared<MenuButton>("START THE GAME ALREADY!");

	m_lobbyPanel->Add(text);
	m_lobbyPanel->Add(text2);
	m_lobbyPanel->Add(start);

}

void Lobby::acceptConnection(const std::string& name)
{
	switch (m_mode)
	{
	case HOST:
	{
		cage::ui::transforms::Move t({ 0.0, m_root.GetSize().y / 2.f + m_lobbyPanel->GetSize().y / 2.0f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
		m_lobbyPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_hostPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
	}
		break;
	case JOIN:
	{
		cage::ui::transforms::Move t({ 0.0, m_root.GetSize().y / 2.f + m_lobbyPanel->GetSize().y / 2.0f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
		m_lobbyPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_connectPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
	}
		break;
	}
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
	switch (m_mode)
	{
	case HOST:
		m_server->name = m_lobbyField->GetText();
		if (m_server->Listen())
		{
			acceptConnection("SERVER");
		}
		break;
	case JOIN:
		if (m_clientConnection)
		{
			UDPpacket* recv = SDLNet_AllocPacket(2048);
			m_clientConnection->Receive(recv);
			using namespace cage::networking;
			using namespace cage::networking::packets;

			std::shared_ptr<MenuButton> joinLobby;
			QueryResponse p;

			switch ((PacketType)recv->data[0])
			{
			case PacketType::QUERY_RESPONSE:
				SDL_memcpy(&p, recv->data, sizeof(p));
				joinLobby = std::make_shared<MenuButton>(std::string("Connect to ") + std::string(p.name));
				joinLobby->Scale(0.4);
				joinLobby->OnClick = [this]()
				{
					ConnectionRequest req;
					m_clientConnection->Send(&req, sizeof(req));
				};
				m_input.Subscribe(joinLobby.get());
				m_connectPanel->Add(joinLobby);
				break;
			case PacketType::CONNECTION_ACCEPT:
				acceptConnection("Joined");
				break;
			}
		}
		break;
	}
}
void Lobby::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	m_spriteShader->Use();
	m_root.Draw();
}