#include "Gameplay.hpp"
#include "../CAGE/Core/StateMachine.hpp"
#include "../CAGE/Core/Game.hpp"


namespace ub
{
	Gameplay::Gameplay(cage::Game& game) : cage::GameState(game)
	{
		auto size = getGame().GetWindow().GetSize();
		m_world = new World(size);
		glViewport(0, 0, size.first, size.second);
	}

	Gameplay::~Gameplay()
	{
		std::cout << "\n\n\n\nDeleting the world\n\n\n\n";
		delete m_world;
	}

	void Gameplay::ProcessEvents()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			m_world->HandleEvents(e);
			switch (e.type)
			{
			case SDL_QUIT:
				quit();
				break;
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					int w = e.window.data1;
					int h = e.window.data2;
					glViewport(0, 0, w, h);
					std::cout << w << " x " << h << std::endl;
				}
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_RETURN)
				{
					quit();
				}
				break;
			}
		}
	}

	void Gameplay::OnHidden()
	{

	}

	void Gameplay::OnRevealed()
	{
		auto [w, h] = getGame().GetWindow().GetSize();
		glViewport(0, 0, w, h);
		m_world->SetWinSize({ w, h });
	}

	void Gameplay::Update(float dt)
	{
		m_world->Update(dt);
	}

	void Gameplay::Draw()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		m_world->Draw();
	}
}