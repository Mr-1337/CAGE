#include "Shrektris.hpp"
#include "ShrekMenu.hpp"
#include <Glad/glad/glad.h>
#include <ctime>
#include <chrono>
#include <GLM/glm/gtc/type_ptr.hpp>
#include <filesystem>

Shrektris::Shrektris(int argc, char** argv) : 
	Game("Shrektris", argc, argv),
	m_running(true)
{

	std::string shrekStr = R"(

                      _____
                   ,-'     `._
                 ,'           `.        ,-.
               ,'               \       ),.\
     ,.       /                  \     /(  \;
    /'\\     ,o.        ,ooooo.   \  ,'  `-')
    )) )`. d8P"Y8.    ,8P"""""Y8.  `'  .--"'
   (`-'   `Y'  `Y8    dP       `'     /
    `----.(   __ `    ,' ,---.       (
           ),--.`.   (  ;,---.        )
          / \O_,' )   \  \O_,'        |
         ;  `-- ,'       `---'        |
         |    -'         `.           |
        _;    ,            )          :
     _.'|     `.:._   ,.::" `..       |
  --'   |   .'     """         `      |`.
        |  :;      :   :     _.       |`.`.-'--.
        |  ' .     :   :__.,'|/       |  \
        `     \--.__.-'|_|_|-/        /   )
         \     \_   `--^"__,'        ,    |
         ;  `    `--^---'          ,'     |
          \  `                    /      /
           \   `    _ _          /
            \           `       /
             \           '    ,'
              `.       ,   _,'
                `-.___.---'
   _____ _              _    _        _
  / ____| |            | |  | |      (_)	
 | (___ | |__  _ __ ___| | _| |_ _ __ _ ___	
  \___ \| '_ \| '__/ _ \ |/ / __| '__| / __|
  ____) | | | | | |  __/   <| |_| |  | \__ \
 |_____/|_| |_|_|  \___|_|\_\\__|_|  |_|___/

	)";

	std::cout << std::endl << shrekStr << std::endl;

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	cage::Texture::MissingTexture = new cage::Texture(IMG_Load("Assets/missing.png"));
    m_stateMachine.Push(new ShrekMenu(*this));
}


void Shrektris::Run()
{
	std::chrono::time_point<std::chrono::high_resolution_clock> currTime;
	std::chrono::duration<float> frameTime(1.0/60.0);

	while (m_running)
	{
		currTime = std::chrono::high_resolution_clock::now();		

		float ft = frameTime.count();

		m_stateMachine.ProcessEvents();
		m_stateMachine.Update(ft);
		m_stateMachine.Draw();
        if (m_stateMachine.Quit())
            m_running = false;

        m_window->SwapBuffers();

		frameTime = std::chrono::high_resolution_clock::now() - currTime;
	}
}