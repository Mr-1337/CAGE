#pragma once

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
	Shrektris();
	~Shrektris();
	void HandleCMDArgs(int argc, char** argv);
	void Init() override;
	void Run() override;

	inline bool IsVR() { return m_vrMode; }
private:


	bool m_vrMode;

	bool m_running;
};