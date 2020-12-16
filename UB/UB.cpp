#include "UB.hpp"
#include <chrono>

#include "GameStates/Gameplay.hpp"
#include "GameStates/MainMenu.hpp"

namespace ub
{
	UndefinedBehavior::UndefinedBehavior(int argc, char** argv) : 
		cage::Game("Undefined Behavior", argc, argv), 
		m_running(true)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		cage::Texture::MissingTexture = new cage::Texture(IMG_Load("Assets/Textures/missing.png"));
		m_window->SetIcon("Assets/Textures/icon.png");
		

		auto size = m_window->GetSize();
		m_stateMachine.Push(new MainMenu(*this));
	}

	void UndefinedBehavior::handleCMDArgs(int argc, char** argv)
	{

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