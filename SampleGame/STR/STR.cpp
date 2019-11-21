#include <GLM/glm/glm.hpp>
#include <GLM/glm/matrix.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_ttf.h>
#include "STR.hpp"
#include "Perlin.hpp"

STR::STR(int argc, char** argv) : 
	Game("STR", argc, argv), 
	m_running(true)
{
	auto size = m_window->GetSize();
	glViewport(0, 0, size.first, size.second);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//glClearColor(0.8f, 0.5f, 0.1f, 1.0f);


}

STR::~STR()
{

}

void STR::Run()
{
	std::chrono::high_resolution_clock clock;
	float deltaTime = 1.0f / 60.f;
	std::chrono::time_point<std::chrono::steady_clock> lastTime = clock.now();
	
	SDL_Event e;

	m_stateMachine.Push(new MainMenu(m_window->GetSize()));

	while (m_running)
	{
		m_running = !m_stateMachine.Quit();
		m_stateMachine.ProcessEvents();
		m_stateMachine.Update(deltaTime);
		m_stateMachine.Draw();
		m_window->SwapBuffers();

		deltaTime = std::chrono::duration<double>(clock.now() - lastTime).count();
		lastTime = clock.now();
	}
}