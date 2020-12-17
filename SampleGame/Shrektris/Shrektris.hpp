#pragma once

#include <array>
#include <string>
#include <memory>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <OpenVR/openvr.h>
#include "../CAGE/CAGE.hpp"

/*
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

*/

class Shrektris : public cage::Game
{
public:
	Shrektris(int argc, char** argv);
	void Run() override;
private:

	void handleCMDArgs(int argc, char** argv) override {}


	bool m_running;
};