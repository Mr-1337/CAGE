#include "Lobby.hpp"
#include "../../CAGE/Core/Game.hpp"

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

	m_connectPanel = std::make_shared <cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 10.f, 20.f }, cage::ui::FlowLayout::Orientation::HORIZONTAL, true));
	auto preText = std::make_shared<cage::ui::Text>(m_font);
	m_ipTextField = std::make_shared<cage::ui::TextField>(m_font, 15);
	auto connect = std::make_shared<MenuButton>("Connect");
	auto backC = std::make_shared<MenuButton>("Back");
	connect->OnClick = [this]()
	{
		acceptConnection(m_ipTextField->GetText());
	};
	backC->OnClick = [this]()
	{
		m_mode = MAIN;
		cage::ui::transforms::Move t({ m_root.GetSize().x / 2.f + m_connectPanel->GetSize().x / 2.0f, 0.f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
		m_buttonGroup->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_connectPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
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

	m_hostPanel = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 10.f, 20.f }, cage::ui::FlowLayout::Orientation::VERTICAL, false));
	m_hostPanel->SetParentMounting(cage::ui::MountPoint::CENTER_RIGHT);
	m_hostPanel->SetLocalMounting(cage::ui::MountPoint::CENTER_LEFT);
	auto backH = std::make_shared<MenuButton>("Back");
	backH->OnClick = [this]()
	{
		m_mode = MAIN;
		cage::ui::transforms::Move t({ m_root.GetSize().x / 2.f + m_hostPanel->GetSize().x / 2.0f, 0.f }, 0.f, 0.5f, cage::ui::transforms::Interpolation::CUBIC);
		m_buttonGroup->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
		m_hostPanel->ScheduleTransform(std::move(std::make_unique<cage::ui::transforms::Move>(t)));
	};
	m_hostPanel->Add(backH);

	auto size = getGame().GetWindow().GetSize();
	m_spriteShader = cage::ui::UIElement::shader;
	m_root.Resize({ size.first, size.second });
	m_root.SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);

	m_root.Add(m_buttonGroup);
	m_root.Add(m_connectPanel);
	m_root.Add(m_hostPanel);

	m_input.Subscribe(host.get());
	m_input.Subscribe(join.get());
	m_input.Subscribe(m_ipTextField.get());
	m_input.Subscribe(connect.get());
	m_input.Subscribe(backC.get());
	m_input.Subscribe(backH.get());
}

void Lobby::acceptConnection(const std::string& name)
{
	auto text = std::make_shared<cage::ui::Text>(m_font);
	text->SetText(name);
	m_hostPanel->Add(text);
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
}
void Lobby::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	m_spriteShader->Use();
	m_root.Draw();
}