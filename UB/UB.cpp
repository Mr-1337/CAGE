#include "UB.hpp"
#include <chrono>

#include "GameStates/MainMenu.hpp"
#include "Battle.hpp"

namespace ub
{
	UndefinedBehavior::UndefinedBehavior() : 
		cage::Game("Undefined Behavior"), 
		m_running(true)
	{
		
	}

	void UndefinedBehavior::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_window->SetIcon("Assets/Textures/icon.png");

		Battle::s_Game = this;
		auto size = m_window->GetSize();
		m_stateMachine.Push(new MainMenu(*this));
	}

	UndefinedBehavior::~UndefinedBehavior()
	{
	}

	void UndefinedBehavior::Run()
	{
		using namespace std::chrono;
		duration<float> frameTime(1.f/60.f);
		time_point<high_resolution_clock> startTime;

		while (m_running)
		{
			startTime = high_resolution_clock::now();
			m_stateMachine.ProcessEvents();
			m_stateMachine.Update(frameTime.count());
			m_stateMachine.Draw();
			m_window->SwapBuffers();
			if (m_stateMachine.Quit())
				m_running = false;

			frameTime = (high_resolution_clock::now() - startTime);
		}
	}

}