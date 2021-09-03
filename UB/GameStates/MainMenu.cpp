#include <SDL2/SDL_image.h>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../CAGE/Core/Platform.hpp"
#include "../CAGE/Core/Game.hpp"
#include "MainMenu.hpp"
#include "Gameplay.hpp"
#include "Editor.hpp"

namespace ub
{
	MainMenu::MainMenu(cage::Game& game) : 
		cage::GameState(game),
		m_font("Assets/Fonts/consola.ttf", 64)
	{

		std::cout << "\n\n\nMain menu allocated\n\n\n";
		cage::TextureManager& m_textureManager = getGame().GetTextureManager();

		m_selection = 0;

		glDisable(GL_DEPTH_TEST);

		auto winSize = getGame().GetWindow().GetSize();

		m_shader = std::make_shared<cage::SpriteShader>();

		using namespace std::chrono_literals;
		m_blinkTimer = 750ms;

		m_shader->Use();

		m_shader->Projection->value = glm::ortho(0.f, (float)winSize.first, (float)winSize.second, 0.f);
		m_shader->Projection->ForwardToShader();

		cage::ui::UIElement::shader = m_shader;
		Entity::s_Game = &getGame();
		DialogueBox::s_Font = new cage::Font("Assets/Fonts/consola.ttf", 20);
		DialogueBox::s_AudioEngine = &getGame().GetPlaybackEngine();

		m_selector = std::make_shared<cage::ui::UIElement>();

		m_root.SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);

		auto& playbackEngine = getGame().GetPlaybackEngine();
		auto& soundManager = getGame().GetSoundManager();

		//playbackEngine.Play(*soundManager.Get("music.mp3"), 0, -1);
		playbackEngine.SetChannelVolume(0, 7);

		// initializing buttons
		{

			SDL_Color color = { 192, 192, 192, 255 };

			const std::string names[4] = { "Play", "Options", "Editor", "Quit" };

			std::function<void(void)> callbacks[4] = {
				[this]() { s_stateMachine->Push(new Gameplay(getGame())); },
				[this, &playbackEngine, &soundManager]() { playbackEngine.Play(*soundManager.Get("XPHardwareDC.ogg"), 1);  },
				[this]() { s_stateMachine->Push(new Editor(getGame())); },
				[this]() { quit(); }
			};

			auto buttonGroup = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::GridLayout({ 0.f, 50.f }, 1));

			for (int i = 0; i < 4; i++)
			{
				auto texture = m_font.Render(names[i], color);
				m_buttons[i] = std::make_shared<cage::ui::Button>(texture, std::nullopt, std::nullopt);
				buttonGroup->Add(m_buttons[i]);
				m_buttons[i]->OnClick = callbacks[i];
				m_buttons[i]->OnHover = [i, this]() { updateSelector(i); };
			}

			m_root.Add(buttonGroup);
			buttonGroup->AddAbsolute(m_selector);
			m_selector->SetActiveTexture(m_font.Render(">", color));
			m_selector->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);
			m_selector->SetLocalMounting(cage::ui::MountPoint::CENTER_RIGHT);
			m_input.Subscribe(&m_root);

		}


		glClearColor(0.0, 0.0, 0.0, 0.0);
		glViewport(0, 0, winSize.first, winSize.second);

		updateSelector(0);

		//m_debug = std::make_shared<cage::ui::DebugVisualizer>(&m_root);
		//m_root.Add(m_debug);
	}

	MainMenu::~MainMenu()
	{
		std::cout << "\n\n\nMain menu dedd\n\n\n";
	}

	void MainMenu::OnRevealed()
	{
		glBindVertexArray(0);
		m_shader->Use();

		auto size = getGame().GetWindow().GetSize();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glViewport(0, 0, size.first, size.second);

		m_root.Resize({ (float)size.first, (float)size.second });

		cage::ui::UIElement::shader = m_shader;

		m_shader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
		m_shader->Projection->ForwardToShader();
	}

	void MainMenu::OnHidden()
	{
	}

	void MainMenu::ProcessEvents()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			m_input.Raise(event);
			switch (event.type)
			{
			case SDL_QUIT:
				quit();
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					auto size = std::make_pair(event.window.data1, event.window.data2);
					glViewport(0, 0, size.first, size.second);
					m_root.Resize({ (float)size.first, (float)size.second });

					m_shader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
					m_shader->Projection->ForwardToShader();
					std::cout << "window resize to " << size.first << " x " << size.second << std::endl;
				}
				break;
			case SDL_KEYDOWN:
				if (!event.key.repeat)
				{
					switch (event.key.keysym.sym)
					{
					case SDLK_w:
					case SDLK_UP:
						--m_selection %= 4;
						updateSelector(m_selection);
						break;

					case SDLK_s:
					case SDLK_DOWN:
						++m_selection %= 4;
						updateSelector(m_selection);
						break;
					case SDLK_RETURN:
						m_buttons[m_selection]->OnClick();
						break;
					}
				}
			}
		}
	}

	void MainMenu::updateSelector(int selection)
	{
		m_selection = selection;
		auto pos = m_buttons[selection]->GetPosition();
		pos.x -= cage::ui::UIElement::GetMountOffset(cage::ui::MountPoint::CENTER_LEFT, m_buttons[selection]->GetSize(), m_buttons[selection]->GetScale()).x;
		m_selector->MoveTo(pos);
	}

	static float time = 0;

	void MainMenu::Update(float dt)
	{
		using namespace std::chrono_literals;
		auto now = std::chrono::high_resolution_clock::now();
		if ((now.time_since_epoch() % (2 * m_blinkTimer)) < m_blinkTimer)
			m_selector->SetVisible(false);
		else
			m_selector->SetVisible(true);
		m_root.Update(dt);
		time += dt;
		//m_scroll->MoveTo({ 0., 100.f * sin(time)});
	}

	void MainMenu::Draw()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		m_shader->Use();
		m_root.Draw();
	}
}