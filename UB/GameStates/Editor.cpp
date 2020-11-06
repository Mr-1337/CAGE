#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../../CAGE/Core/Game.hpp"
#include "Editor.hpp"
#include "../CAGE/Graphics/UI/FlowLayout.hpp"
#include "../CAGE/Graphics/UI/GridLayout.hpp"

namespace ub
{
	Editor::Editor(cage::Game& game) : 
		cage::GameState(game), 
		m_playback(false),
		m_currentTool(nullptr)
	{
		m_world = std::make_unique<World>(std::pair{ 16, 16 });
		auto size = getGame().GetWindow().GetSize();

		auto& lang = getGame().GetLanguage();
		lang.LoadFile("editor.lang");

		m_world->SetWinSize(size);
		m_rightPanel = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::GridLayout({ 0.f, 0.f }, 1));
		m_rightPanel->SetParentMounting(cage::ui::MountPoint::TOP_RIGHT);
		m_rightPanel->SetLocalMounting(cage::ui::MountPoint::TOP_RIGHT);

		m_pencilTool = std::make_unique<Pencil>(this, m_world.get());
		m_handTool = std::make_unique<Hand>(this, m_world.get());
		m_currentTool = m_pencilTool.get();

		//m_rightPanel->MoveTo({ 0.f, 100.f });

		m_tilePanel = std::make_shared<TilePanel>(m_world.get());

		auto font = cage::Font("Assets/Fonts/consola.ttf", 16);
		glClearColor(0.0, 0.0, 0.0, 1.0);

		m_tabBar = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 5.f, 0.f }, true));
		//m_tabBar->Scale(0.25f);

		m_tabBar->Add(std::make_shared<EditorTab>(lang.Get("ui.tabs.tile"), font));
		m_tabBar->Add(std::make_shared<EditorTab>(lang.Get("ui.tabs.structure"), font));
		m_tabBar->Add(std::make_shared<EditorTab>(lang.Get("ui.tabs.entity"), font));
		m_tabBar->Add(std::make_shared<EditorTab>(lang.Get("ui.tabs.script"), font));

		//m_tabBar->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);
		//m_tabBar->SetLocalMounting(cage::ui::MountPoint::BOTTOM_LEFT);

		m_rightPanel->Add(m_tabBar);
		m_rightPanel->Add(m_tilePanel);
		//m_rightPanel->Resize(m_tilePanel->GetSize());

		std::for_each(m_rightPanel->GetChildren().begin(), m_rightPanel->GetChildren().end(), [this](auto c) { m_input.Subscribe(c.get()); });
		std::for_each(m_tilePanel->GetChildren().begin(), m_tilePanel->GetChildren().end(), [this](auto c) { m_input.Subscribe(c.get()); });

		m_bottomPanel = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::GridLayout({ 0.f, 15.f }, 1));
		m_bottomPanel->SetParentMounting(cage::ui::MountPoint::BOTTOM);
		m_bottomPanel->SetLocalMounting(cage::ui::MountPoint::BOTTOM);

		auto& tex = getGame().GetTextureManager();
		m_play = std::make_shared<cage::ui::Button>(tex.Get("Play.png"), std::nullopt, std::nullopt);
		m_pause = std::make_shared<cage::ui::Button>(tex.Get("Pause.png"), std::nullopt, std::nullopt);
		m_stop = std::make_shared<cage::ui::Button>(tex.Get("Stop.png"), std::nullopt, std::nullopt);


		auto pbBar = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 15.f, 0.f }));

		pbBar->Add(m_play);
		pbBar->Add(m_pause);
		pbBar->Add(m_stop);

		//m_pause->Scale(0.1f);
		pbBar->Compress();

		m_bottomPanel->Add(pbBar);

		auto toolBar = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 20.f, 0.f }));
		m_hand = std::make_shared<cage::ui::Button>(tex.Get("hand.png"), std::nullopt, std::nullopt);
		m_pencil = std::make_shared<cage::ui::Button>(tex.Get("pencil.png"), std::nullopt, std::nullopt);
		m_file = std::make_shared<cage::ui::Button>(tex.Get("save.png"), std::nullopt, std::nullopt);
		m_gridToggle = std::make_shared<cage::ui::CheckBox>(getGame().GetTextureManager(), lang.Get("ui.checkbox.grid"), font);

		auto hovHigh = std::make_shared<cage::ui::UIElement>();
		hovHigh->Resize(m_hand->GetScaledSize() * 1.3f);
		hovHigh->SetVisible(false);
		hovHigh->SetColor({ 0.3, 0.0, 0.7, 0.5 });

		auto selHigh = std::make_shared<cage::ui::UIElement>();
		selHigh->Resize(m_hand->GetScaledSize() * 1.3f);
		selHigh->SetParentMounting(cage::ui::MountPoint::CENTER_LEFT);
		selHigh->SetLocalMounting(cage::ui::MountPoint::CENTER_LEFT);
		selHigh->SetVisible(true);
		selHigh->SetColor({ 0, 0.5, 0, 0.5 });

		m_hand->OnClick = [this, selHigh]()		{ m_currentTool = m_handTool.get();		selHigh->MoveTo(m_hand->GetPosition()); };
		m_pencil->OnClick = [this, selHigh]()	{ m_currentTool = m_pencilTool.get();	selHigh->MoveTo(m_pencil->GetPosition()); };

		toolBar->Add(m_pencil);
		toolBar->Add(m_hand);
		toolBar->Add(m_file);
		toolBar->Add(m_gridToggle);
		toolBar->AddAbsolute(selHigh);

		m_play->OnClick = [this, toolBar]() ->void
		{
			m_playback = true;
		};

		m_pause->OnClick = [this, toolBar]()
		{
			m_playback = false;
		};

		m_stop->OnClick = [this, toolBar]()
		{
			m_playback = false;
			m_world->Reload();
		};

		std::for_each(toolBar->GetChildren().begin(), toolBar->GetChildren().end(), [this](auto c) { m_input.Subscribe(c.get()); });
		
		m_bottomPanel->Add(toolBar);
		m_bottomPanel->Compress();
		//m_bottomPanel->MoveTo({ 0.f, -100.f });

		m_input.Subscribe(m_play.get());
		m_input.Subscribe(m_pause.get());
		m_input.Subscribe(m_stop.get());

		m_root.SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);
		m_root.Resize({ size.first, size.second });
		//m_root.MoveTo({ size.first / 2, size.second / 2 });
		m_root.Add(m_rightPanel);
		m_root.Add(m_bottomPanel);

		m_gameViewport = { size.first, size.second };
		m_gameViewport -= glm::vec2{ m_rightPanel->GetScaledSize().x, m_bottomPanel->GetScaledSize().y };
		m_world->SetWinSize({ m_gameViewport.x, m_gameViewport.y });
		m_world->Resize({ 256, 256 });

		glm::vec4
			bgColor = { 0.2, 0.2, 0.2, 1.0 },
			fgColor = { 0.35, 0.35, 0.35, 1.0 },
			panelColor = { 0.47, 0.47, 0.47, 1.0 };

		m_rightPanel->SetColor(bgColor);
		m_bottomPanel->SetColor(bgColor);
		m_tabBar->SetColor(fgColor);
		m_tilePanel->SetColor(fgColor);

		pbBar->SetColor(fgColor);
		toolBar->SetColor(fgColor);

		std::ifstream file;
		file.open("Assets/Worlds/shrek.txt");

		for (int i = 0; i < 48; i++)
		{
			for (int j = 0; j < 48; j++)
			{
				std::string s;
				file >> s;
				m_world->PaintTileRaw(j, i, (World::Tile)std::stoi(s));
			}
		}

		m_tileHighlight = std::make_shared<cage::ui::UIElement>();
		m_tileHighlight->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);
		m_tileHighlight->SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);
		m_tileHighlight->Resize({ World::TILE_SIZE, World::TILE_SIZE });
		m_tileHighlight->SetColor({ 1.0, 1.0, 1.0, 0.3 });

		m_root.Add(m_tileHighlight);

		glViewport(0, 0, size.first, size.second);

	}

	Editor::~Editor()
	{

	}

	void Editor::OnRevealed()
	{

	}

	void Editor::OnHidden()
	{

	}

	void Editor::ProcessEvents()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			m_input.Raise(event);
			if (!m_playback)
				m_currentTool->Use(m_input.Convert(event));

			if (m_playback)
				m_world->HandleEvents(event);
			switch (event.type)
			{
			case SDL_QUIT:
				quit();
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					glViewport(0, 0, event.window.data1, event.window.data2);
					auto size = std::make_pair(event.window.data1, event.window.data2);
					m_root.Resize({ size.first, size.second });
					
					m_gameViewport = { size.first, size.second };
					m_gameViewport -= glm::vec2{ m_rightPanel->GetScaledSize().x, m_bottomPanel->GetScaledSize().y };
					m_world->SetWinSize({m_gameViewport.x, m_gameViewport.y});
					//m_root.MoveTo({ size.first / 2, size.second / 2 });
					cage::ui::UIElement::shader->Use();
					cage::ui::UIElement::shader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
					cage::ui::UIElement::shader->Projection->ForwardToShader();
				}
				break;
			}
		}
	}

	void Editor::Update(float dt)
	{
		if (m_playback)
			m_world->Update(dt);
		else
		{
			glm::vec2 displacement = { 0.f, 0.f };
			glm::vec2 velocity = { 0.f, 0.f };

			auto keys = SDL_GetKeyboardState(nullptr);
			if (keys[SDL_SCANCODE_W])
				velocity.y = -1;
			if (keys[SDL_SCANCODE_S])
				velocity.y = 1;
			if (keys[SDL_SCANCODE_A])
				velocity.x = -1;
			if (keys[SDL_SCANCODE_D])
				velocity.x = 1;

			float speed = 1000.f;
			displacement = velocity * speed * dt;

			m_world->Move(displacement);
		}
		m_world->SetGrid(m_gridToggle->Checked());
	}

	void Editor::Draw()
	{
		
		auto size = getGame().GetWindow().GetSize();
		glViewport(0, size.second - m_gameViewport.y, m_gameViewport.x, m_gameViewport.y);

		glClear(GL_COLOR_BUFFER_BIT);

		m_world->Draw();
		glViewport(0, 0, size.first, size.second);

		cage::ui::UIElement::shader->Use();
		m_root.Draw();
	}
}