#include <SDL2/SDL_image.h>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../CAGE/Core/StateMachine.hpp"
#include "../CAGE/Core/Platform.hpp"
#include "../CAGE/Core/Game.hpp"
#include "MainMenu.hpp"
#include "Gameplay.hpp"
#include "Editor.hpp"

namespace ub
{
	MainMenu::MainMenu(cage::Game& game) : 
		cage::GameState(game),
		k_buttonSpacing(50.f)
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

		m_selector = std::make_shared<cage::ui::UIElement>();
		m_selector->SetActiveTexture(m_textureManager.Get("MenuSelector.png"));

		m_root.SetLocalMounting(cage::ui::MountPoint::CENTER);
		m_root.Resize({ (float)winSize.first, (float)winSize.second });
		m_root.MoveTo({ (float)winSize.first / 2, (float)winSize.second / 2 });

		
		std::cout << cage::platform::GetUsername() << std::endl;

		auto& playbackEngine = getGame().GetPlaybackEngine();
		auto& soundManager = getGame().GetSoundManager();

		playbackEngine.Play(*soundManager.Get("music.mp3"), 0, -1);
		playbackEngine.SetChannelVolume(0, 7);

		// initializing buttons
		{
			const std::string names[4] = { "MenuPlay.png", "MenuOptions.png", "MenuEditor.png", "MenuQuit.png" };

			std::function<void(void)> callbacks[4] = {
				[this]() { s_stateMachine->Push(new Gameplay(getGame())); },
				[this, &playbackEngine, &soundManager]() { playbackEngine.Play(*soundManager.Get("XPHardwareDC.ogg"), 1);  },
				[this]() { s_stateMachine->Push(new Editor(getGame())); },
				[this]() { quit(); }
			};

			for (int i = 0; i < 4; i++)
			{
				m_buttons[i] = std::make_shared<cage::ui::Button>(m_textureManager.Get(names[i]), std::nullopt, std::nullopt);
				m_root.Add(m_buttons[i]);
				m_input.Subscribe(m_buttons[i].get());
				m_buttons[i]->Scale(0.25f);
				m_buttons[i]->MoveTo({ 0.f, k_buttonSpacing * (-1.5f + i) });
				m_buttons[i]->OnClick = callbacks[i];
				m_buttons[i]->OnHover = [i, this]() { updateSelector(i); };
			}
		}

		m_root.Add(m_selector);
		m_selector->Scale(0.25f);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glViewport(0, 0, winSize.first, winSize.second);

		updateSelector(0);

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
		m_root.MoveTo({ (float)size.first / 2, (float)size.second / 2 });

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
					m_root.MoveTo({ (float)size.first / 2, (float)size.second / 2 });

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
		m_selector->MoveTo({ -25.f, k_buttonSpacing * (-1.5f + m_selection) });
	}

	void MainMenu::Update(float dt)
	{
		using namespace std::chrono_literals;
		auto now = std::chrono::high_resolution_clock::now();
		if ((now.time_since_epoch() % (2 * m_blinkTimer)) < m_blinkTimer)
			m_selector->SetVisible(false);
		else
			m_selector->SetVisible(true);
		m_root.Update(dt);
	}

	void MainMenu::Draw()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		m_shader->Use();
		m_root.Draw();
	}
}