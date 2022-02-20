#include <GLM/glm/gtc/matrix_transform.hpp>
#include <Graphics/UI/TextField.hpp>
#include "../../CAGE/Core/Game.hpp"
#include "Editor.hpp"
#include "../CAGE/Graphics/UI/FlowLayout.hpp"
#include "../CAGE/Graphics/UI/GridLayout.hpp"
#include "../World/WorldSerializer.hpp"

namespace ub
{
	Editor::Editor(cage::Game& game) : 
		cage::GameState(game), 
		m_playback(false),
		m_currentTool(nullptr),
		m_world(std::make_unique<World>(std::pair{ 64, 64 })),
		m_font("Assets/Fonts/consola.ttf", 18)
	{
		const auto size = getGame().GetWindow().GetSize();

		auto& lang = getGame().GetLanguage();
		lang.LoadFile("editor.lang");
		m_world->SetWinSize(size);
		m_rightPanel = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 0.f, 0.f }, cage::ui::FlowLayout::Orientation::VERTICAL, false));
		m_rightPanel->SetParentMounting(cage::ui::MountPoint::TOP_RIGHT);
		m_rightPanel->SetLocalMounting(cage::ui::MountPoint::TOP_RIGHT);

		m_pencilTool = std::make_unique<Pencil>(this, m_world.get());
		m_handTool = std::make_unique<Hand>(this, m_world.get());
		m_currentTool = m_pencilTool.get();

		//m_rightPanel->MoveTo({ 0.f, 100.f });

		m_tilePanel = std::make_shared<TilePanel>(m_world.get());

		glClearColor(0.0, 0.0, 0.0, 0.0);

		m_tabBar = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 5.f, 0.f }, true));
		//m_tabBar->Scale(0.25f);

		m_tabBar->Add(std::make_shared<EditorTab>(lang.Get("ui.tabs.tile"), m_font));
		m_tabBar->Add(std::make_shared<EditorTab>(lang.Get("ui.tabs.structure"), m_font));
		m_tabBar->Add(std::make_shared<EditorTab>(lang.Get("ui.tabs.entity"), m_font));
		m_tabBar->Add(std::make_shared<EditorTab>(lang.Get("ui.tabs.script"), m_font));

		m_rightPanel->Add(m_tabBar);
		m_rightPanel->Add(m_tilePanel);


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
		m_gridToggle = std::make_shared<cage::ui::CheckBox>(getGame().GetTextureManager(), lang.Get("ui.checkbox.grid"), m_font);

		auto hovHigh = std::make_shared<cage::ui::UIElement>();
		hovHigh->Resize(m_hand->GetScaledSize() * 1.3f);
		hovHigh->SetVisible(false);
		hovHigh->SetColor({ 0.3, 0.0, 0.7, 0.5 });

		auto selHigh = std::make_shared<cage::ui::UIElement>();
		selHigh->Resize(m_hand->GetScaledSize() * 1.3f);
		selHigh->SetParentMounting(cage::ui::MountPoint::CENTER_LEFT);
		selHigh->SetVisible(true);
		selHigh->SetColor({ 0, 0.5, 0, 0.5 });

		auto select = [this, selHigh](std::shared_ptr<cage::ui::UIElement> button)
		{
			glm::vec2 pos = button->GetPosition();
			pos += cage::ui::UIElement::GetMountOffset(cage::ui::MountPoint::CENTER_LEFT, button->GetSize(), button->GetScale());
			selHigh->MoveTo(pos);	
		};

		auto selectTool = [this, select](std::shared_ptr<cage::ui::UIElement> button, Tool* tool)
		{
			select(button);
			if (!m_currentTool)
				m_saveDialogue->SetVisible(false);
			m_currentTool = tool;
		};

		m_hand->OnClick = [this, selectTool]()	  { selectTool(m_hand, m_handTool.get()); };
		m_pencil->OnClick = [this, selectTool]() { selectTool(m_pencil, m_pencilTool.get()); };
		m_file->OnClick = [this, select]() { toggleSaveUI(); select(m_file); m_currentTool = nullptr; };

		toolBar->Add(m_pencil);
		toolBar->Add(m_hand);
		toolBar->Add(m_file);
		toolBar->Add(m_gridToggle);
		selectTool(m_hand, m_handTool.get());
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

		
		m_bottomPanel->Add(toolBar);
		m_bottomPanel->Compress();

		//m_bottomPanel->MoveTo({ 0.f, -100.f });


		m_root.SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);
		m_root.Resize({ size.first, size.second });

		m_root.Add(m_rightPanel);
		m_root.Add(m_bottomPanel);

		m_gameViewport = { size.first, size.second };
		m_gameViewport -= glm::vec2{ m_rightPanel->GetScaledSize().x, m_bottomPanel->GetScaledSize().y };
		m_world->SetWinSize({ m_gameViewport.x, m_gameViewport.y });
		m_world->Resize({ 64, 64 });

		glm::vec4
			bgColor = { 0.2, 0.2, 0.2, 1.0 },
			fgColor = { 0.35, 0.35, 0.35, 1.0 },
			panelColor = { 0.47, 0.47, 0.47, 1.0 };

		m_rightPanel->SetColor(bgColor);
		//m_rightPanel->SetMasking(true);
		m_bottomPanel->SetColor(bgColor);
		m_tabBar->SetColor(fgColor);
		m_tilePanel->SetColor(fgColor);

		pbBar->SetColor(fgColor);
		toolBar->SetColor(fgColor);
		
		{
			using namespace cage::ui;
			m_saveDialogue = std::make_shared<LayoutGroup>(new FlowLayout({}, FlowLayout::Orientation::VERTICAL, false));
			m_saveDialogue->SetColor(bgColor);

			auto sdTopbar = std::make_shared<LayoutGroup>(new FlowLayout());
			//auto sdTopbar = std::make_shared<UIElement>();
			sdTopbar->SetColor(fgColor);
			sdTopbar->SetRelativeSizeAxes(Axis::HORIZONTAL);
			sdTopbar->Resize({ 1.0f, 80.f });

			auto text = std::make_shared<Text>(m_font);
			text->SetText("Save / Load World");
			auto textBG = std::make_shared<UIElement>();
			textBG->SetRelativeSizeAxes(Axis::BOTH);
			textBG->Add(text);
			textBG->Resize({ 0.75f, 1.0f });

			auto closeButton = std::make_shared<Button>(std::nullopt, std::nullopt, std::nullopt);
			closeButton->SetColor({ 0.7, 0.2, 0.2, 1.0 });
			closeButton->SetRelativeSizeAxes(Axis::BOTH);

			closeButton->Resize({ 0.25f, 1.0f });

			closeButton->OnClick = [this]() { toggleSaveUI(); };
			closeButton->OnHover = [closeButton]() { closeButton->HaltAllTransforms(); closeButton->ScheduleTransform(std::make_unique<transforms::FadeTo>(glm::vec4{ 0.8, 0.3, 0.3, 1.0 }, 0.f, 0.1f)); };
			closeButton->OnUnHover = [closeButton]() { closeButton->HaltAllTransforms(); closeButton->ScheduleTransform(std::make_unique<transforms::FadeTo>(glm::vec4{ 0.7, 0.2, 0.2, 1.0 }, 0.f, 0.1f)); };

			auto line = std::make_shared<UIElement>();
			line->SetColor({ 1.0, 1.0, 1.0, 1.0 });
			line->Resize({ 20.0, 2.0 });
			line->Rotate(glm::radians(45.f));

			closeButton->Add(line);

			line = std::make_shared<UIElement>();
			line->SetColor({ 1.0, 1.0, 1.0, 1.0 });
			line->Resize({ 20.0, 2.0 });
			line->Rotate(glm::radians(-45.f));

			closeButton->Add(line);

			sdTopbar->Add(textBG);
			sdTopbar->Add(closeButton);
			m_saveDialogue->Add(sdTopbar);


			auto filenameRow = std::make_shared<LayoutGroup>(new FlowLayout());
			auto filenameField = std::make_shared<TextField>(m_font, 40);
			text = std::make_shared<Text>(m_font);
			text->SetColor({ 1.0, 1.0, 1.0, 1.0 });
			text->SetText("File Name: ");
			filenameRow->Add(text);
			filenameRow->Add(filenameField);
			m_saveDialogue->Add(filenameRow);

			auto buttonBar = std::make_shared<LayoutGroup>(new FlowLayout());
			buttonBar->SetRelativeSizeAxes(Axis::HORIZONTAL);
			buttonBar->Resize({ 1.0, 80.0f });

			auto saveButton = std::make_shared<Button>(std::nullopt, std::nullopt, std::nullopt);
			saveButton->SetColor({ 0.7, 0.2, 0.2, 1.0 });
			saveButton->SetRelativeSizeAxes(Axis::BOTH);
			saveButton->Resize({ 0.5, 1.0 });

			saveButton->OnClick = [this, filenameField]() {
				WorldSerializer serializer(*m_world);
				serializer.SaveToFile(filenameField->GetText());
			};

			auto loadButton = std::make_shared<Button>(std::nullopt, std::nullopt, std::nullopt);
			loadButton->SetColor({ 0.2, 0.7, 0.2, 1.0 });
			loadButton->SetRelativeSizeAxes(Axis::BOTH);
			loadButton->Resize({ 0.5, 1.0 });

			loadButton->OnClick = [this, filenameField]() {
				WorldSerializer serializer(*m_world);
				serializer.LoadFromFile(filenameField->GetText());
			};

			text = std::make_shared<cage::ui::Text>(m_font);
			text->SetColor({ 1.0, 1.0, 1.0, 1.0 });
			text->SetText("Save");
			saveButton->Add(text);

			text = std::make_shared<cage::ui::Text>(m_font);
			text->SetColor({ 1.0, 1.0, 1.0, 1.0 });
			text->SetText("Load");
			loadButton->Add(text);

			buttonBar->Add(saveButton);
			buttonBar->Add(loadButton);

			m_saveDialogue->Add(buttonBar);
			buttonBar->Update();
			sdTopbar->Update();

			m_root.Add(m_saveDialogue);
			m_saveDialogue->SetVisible(false);
			//m_saveDialogue->SetRelativePositionAxes(Axis::BOTH);
		}

		std::ifstream file;
		file.open("Assets/Worlds/gorbo.txt");

		for (int i = 0; i < 64; i++)
		{
			for (int j = 0; j < 64; j++)
			{
				std::string s;
				file >> s;
				m_world->PaintTileRaw(j, i, (Tilemap::Tile)std::stoi(s));
			}
		}

		m_tileHighlight = std::make_shared<cage::ui::UIElement>();
		m_tileHighlight->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);
		m_tileHighlight->SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);
		m_tileHighlight->Resize({ Tilemap::TILE_SIZE, Tilemap::TILE_SIZE });
		m_tileHighlight->SetColor({ 1.0, 1.0, 1.0, 0.3 });

		m_root.Add(m_tileHighlight);

		m_gameRoot = std::make_shared<cage::ui::UIElement>();
		//m_root.Add(m_gameRoot);
		m_gameRoot->Resize(m_gameViewport);
		m_gameRoot->SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);
		m_gameRoot->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);

		//m_debugVisualizer = std::make_shared<cage::ui::DebugVisualizer>(&m_root);

		//m_root.Add(m_debugVisualizer);
		m_input.Subscribe(&m_root);

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
			if (!m_playback && m_currentTool)
				m_currentTool->Use(m_input.Convert(event));

			m_world->HandleEvents(m_input.Convert(event));

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

					m_world->SetWinSize({ m_gameViewport.x, m_gameViewport.y });

					//m_root.MoveTo({ size.first / 2, size.second / 2 });
					cage::ui::UIElement::shader->Use();
					cage::ui::UIElement::shader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
					cage::ui::UIElement::shader->Projection->ForwardToShader();
				}
				break;
			case SDL_KEYDOWN:
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
		m_root.Update(dt);
		m_gameRoot->Update(dt);
	}

	void Editor::Draw()
	{
		
		auto size = getGame().GetWindow().GetSize();
		glViewport(0, size.second - m_gameViewport.y, m_gameViewport.x, m_gameViewport.y);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		cage::ui::UIElement::shader->Use();
		cage::ui::UIElement::shader->Projection->value = glm::ortho(0.f, (float)m_gameViewport.x, (float)m_gameViewport.y, 0.f);
		cage::ui::UIElement::shader->Projection->ForwardToShader();

		m_world->Draw();

		glViewport(0, 0, size.first, size.second);
		cage::ui::UIElement::shader->Use();
		cage::ui::UIElement::shader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
		cage::ui::UIElement::shader->Projection->ForwardToShader();
		glDisable(GL_DEPTH_TEST);
		m_root.Draw();
	}

	void Editor::toggleSaveUI()
	{
		bool visible = m_saveDialogue->IsVisible();
		m_saveDialogue->SetVisible(!visible);
	}
}
