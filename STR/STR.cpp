#include <GLM/glm/glm.hpp>
#include <GLM/glm/matrix.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_ttf.h>
#include "STR.hpp"
#include "Perlin.hpp"

STR::STR() : 
	Game("ShrekTech Racing")
{	
	//glClearColor(0.8f, 0.5f, 0.1f, 1.0f);


}

STR::~STR()
{

}

void STR::Init()
{
	auto size = m_window->GetSize();
	glViewport(0, 0, size.first, size.second);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void STR::HandleCMDArgs(int argc, char** argv)
{

}

void STR::Run()
{
	std::chrono::high_resolution_clock clock;
	float deltaTime = 1.0f / 60.f;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = clock.now();
	
	SDL_Event e;

	m_stateMachine.Push(new MainMenu(*this, m_window->GetSize()));

	while (!m_stateMachine.Quit())
	{
		m_stateMachine.ProcessEvents();
		m_stateMachine.Update(deltaTime);
		m_stateMachine.Draw();
		m_window->SwapBuffers();

		deltaTime = std::chrono::duration<double>(clock.now() - lastTime).count();
		lastTime = clock.now();
	}
}