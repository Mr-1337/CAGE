#include "Shrektris.hpp"
#include <Glad/glad/glad.h>
#include <ctime>
#include <chrono>

Shrektris::Shrektris(int argc, char** argv) : 
	Game("Shrektris", argc, argv),
	m_running(true), 
	vertexShader(cage::Shader::VERTEX), 
	fragShader(cage::Shader::FRAGMENT),
	spriteVS(cage::Shader::VERTEX),
	spriteFS(cage::Shader::FRAGMENT)
{
	auto size = m_window->GetSize();
	glViewport(0, 0, size.first, size.second);

	m_level = 1;
	m_score = 0;
	m_levelCounter = 0;
	levelTime = 1.5f - (m_level - 1) * 0.1f;
	totalTime = 0;

	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	cage::Texture::MissingTexture = new cage::Texture(IMG_Load("Assets/missing.png"));

	shrek.LoadModel("Assets/shrek.obj");
	thanos.LoadModel("Assets/thanos.obj");
	m_font = TTF_OpenFont("Assets/sans.ttf", 32);

	std::srand(std::time(nullptr));

	std::string vsString = R"REE(
	#version 460 core

	layout (location = 0) in vec3 pos;
	layout (location = 1) in vec2 uv;
	layout (location = 2) in vec3 norm;

	uniform mat4 u_projection;
	uniform mat4 u_view;
	uniform mat4 u_model;

	out vec4 pos_o;
	out vec3 pos_world_o;
	out vec3 norm_o;
	out vec2 uv_o;

	void main()
	{
		pos_o = (u_projection * u_view * u_model * vec4(pos, 1.0));
		pos_world_o = (u_model * vec4(pos, 1.0)).xyz;
		norm_o = norm;
		uv_o = vec2(uv.x, -uv.y);
		gl_Position = pos_o;
	}
	)REE";

	std::string fsString = R"REE(
	#version 460 core

	in vec4 pos_o;
	in vec3 pos_world_o;
	in vec2 uv_o;
	in vec3 norm_o;
	out vec4 colorOut;

	uniform sampler2D u_texture;

	void main()
	{
		float diffuse = max(dot(norm_o, normalize(-pos_world_o + vec3(0, 100, 0))), 0.2);
		colorOut = vec4(diffuse, diffuse, diffuse, 1.0) * texture(u_texture, uv_o);
	}
	)REE";

	vertexShader.CompileFromSrcString(vsString);
	fragShader.CompileFromSrcString(fsString);

	program = std::make_unique<cage::Generic3DShader>(vertexShader, fragShader);

	program->Use();

	program->Projection->value = glm::perspective(glm::quarter_pi<float>(), (float)size.first / (float)size.second, 0.1f, 100.f);
	program->Projection->ForwardToShader();
	program->Model->value = glm::scale(program->Model->value, { 1.f, 0.6f, 1.f });
	program->Model->ForwardToShader();

	spriteVS.CompileFromFile("Assets/sprite.ver");
	spriteFS.CompileFromFile("Assets/sprite.frag");

	spriteProgram = std::make_shared<cage::SpriteShader>(spriteVS, spriteFS);
	spriteProgram->Use();

	spriteProgram->Projection->value = glm::ortho(0.0f, (float)size.first, (float)size.second, 0.0f);
	spriteProgram->Projection->ForwardToShader();
	cage::ui::UIElement::shader = spriteProgram;

	music = Mix_LoadWAV("Assets/music.mp3");

	Mix_VolumeChunk(music, 10);
	
	layers1 = Mix_LoadWAV("Assets/layers1.ogg");
	layers2 = Mix_LoadWAV("Assets/layers2.ogg");
	layers3 = Mix_LoadWAV("Assets/layers3.ogg");
	bigLayers = Mix_LoadWAV("Assets/biglayers.ogg");
	donkey = Mix_LoadWAV("Assets/donkey.ogg");

	levels[0] = Mix_LoadWAV("Assets/level1.ogg");
	levels[1] = Mix_LoadWAV("Assets/level2.ogg");
	levels[2] = Mix_LoadWAV("Assets/level3.ogg");
	levels[3] = Mix_LoadWAV("Assets/level4.ogg");

	Mix_PlayChannel(2, music, -1);

	currentPiece.shape = 0;
	currentPiece.y = 1;
	currentPiece.x = 5;

	glClearColor(0.3f, 0.5f, 0.2f, 0.0f);

	scoreText = std::make_shared<cage::ui::UIElement>();
	levelText = std::make_shared<cage::ui::UIElement>();

	m_rootNode.SetMounting(cage::ui::MountPoint::TOP_LEFT);
	scoreText->SetMounting(cage::ui::MountPoint::TOP_LEFT);
	levelText->SetMounting(cage::ui::MountPoint::TOP_LEFT);

	scoreText->MoveTo({ 250.f, 0.f });
	levelText->MoveTo({ 250.f, 50.f });

	m_rootNode.Resize({ (float)size.first, (float)size.second });
	//m_rootNode.LoadTexture(IMG_Load("Assets/simon.png"));

	m_rootNode.Add(scoreText);
	m_rootNode.Add(levelText);

	scoreText->LoadTexture(TTF_RenderText_Blended(m_font, "Score: 0 ", fontColor));
	levelText->LoadTexture(TTF_RenderText_Blended(m_font, "Level: 1", fontColor));


}

bool spin = false;

void Shrektris::Run()
{
	SDL_Event e;
	auto startTime = std::chrono::system_clock::now();
	
	bool wireframe = false;
	float t = 0;

	std::chrono::time_point<std::chrono::high_resolution_clock> currTime;
	std::chrono::duration<float> frameTime(1.0f/60.0f);

	while (m_running)
	{
		currTime = std::chrono::high_resolution_clock::now();
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				m_running = false;
			}
			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.scancode == SDL_SCANCODE_TAB && e.key.repeat == false)
					if (SDL_GetRelativeMouseMode() == SDL_TRUE)
						SDL_SetRelativeMouseMode(SDL_FALSE);
					else
						SDL_SetRelativeMouseMode(SDL_TRUE);
				else if (e.key.keysym.scancode == SDL_SCANCODE_F && e.key.repeat == false)
				{
					wireframe = !wireframe;
					glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_P && e.key.repeat == false)
				{
					spin = !spin;
					position = glm::vec3(0, 2, 20), target = glm::vec3(0, 0, -1);
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_A)
				{
					movePieceLR(currentPiece, -1);
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_D)
				{
					movePieceLR(currentPiece, 1);
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_S)
				{
					if (!movePieceDown(currentPiece))
						t = levelTime;
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_E)
				{
					rotate(currentPiece);
				}
			}

		}

		auto keys = SDL_GetKeyboardState(nullptr);
		const float speed = 6;
		glm::vec3 velocity{ 0.f, 0.f, 0.f };

		if (!spin)
		{
			if (keys[SDL_SCANCODE_J])
			{
				velocity.x = -speed;
			}

			if (keys[SDL_SCANCODE_K])
			{
				velocity.z = speed;
			}

			if (keys[SDL_SCANCODE_L])
			{
				velocity.x = speed;
			}

			if (keys[SDL_SCANCODE_I])
			{
				velocity.z = -speed;
			}

			if (keys[SDL_SCANCODE_U])
			{
				velocity.y = speed;
			}

			if (keys[SDL_SCANCODE_O])
			{
				velocity.y = -speed;
			}

			position += velocity * frameTime.count();
		}

		float ft = frameTime.count();

		t += ft;
		totalTime += ft;
		logic(t);
		draw(ft);
		frameTime = std::chrono::high_resolution_clock::now() - currTime;

	}

	auto duration = std::chrono::system_clock::now() - startTime;
	long long seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
	int minutes = seconds / 60;
	seconds = seconds % 60;
	std::string z = ":";
	if (seconds < 10)
		z = ":0";
	std::cout << "You survived for " << minutes << z << seconds % 60 << std::endl;
}

void Shrektris::logic(float& t)
{
	visualizer = currentPiece;
	visualizer.y += 4;

	while (checkCollision(visualizer, getSquares(visualizer)))
	{
		visualizer.y++;
	}

	visualizer.y--;

	if (t >= levelTime)
	{
		t = 0.f;
		m_levelCounter++;
		if (m_levelCounter > 60)
		{
			m_level++;
			levelTime -= 0.1f;
			m_levelCounter = 0;
			Mix_PlayChannel(5, levels[rand() % 4], 0);
			levelText->LoadTexture(TTF_RenderText_Blended(m_font, std::string("Level: ").append(std::to_string(m_level)).c_str(), fontColor));
		}
		std::cout << "MOVE" << std::endl;
		if (!movePieceDown(currentPiece))
		{
			if (currentPiece.y == 0) // You lost :(
			{
				Mix_HaltChannel(2);
				Mix_HaltChannel(5);
				Mix_PlayChannel(4, donkey, 0);
				SDL_Delay(7000);
				m_running = false;
				return;
			}
			std::cout << "FAILURE!!!" << std::endl;
			SDL_Delay(500);

			//Clear any rows we should

			int breakTotal = 0;
			for (size_t i = 14; i > 0; i--)
			{

				int yesCount = 0;
				for (int j = 0; j < 10; j++)
				{
					if (board[i][j] != 0)
						yesCount++;
				}
				if (yesCount == 10)
				{
					// we cleared the row

					// copy the board into itself but 1 row lower
					memcpy(&board[1][0], board, (i * 10 * sizeof(int)));
					// zero the now empty top
					memset(board, 0, (10 * sizeof(int)));
					i++;
					breakTotal++;
				}

			}

			if (breakTotal == 1)
			{
				m_score += 40 * m_level;
				Mix_PlayChannel(3, layers1, 0);
			}
			else if (breakTotal == 2)
			{
				m_score += 100 * m_level;
				Mix_PlayChannel(3, layers2, 0);
			}
			else if (breakTotal == 3)
			{
				m_score += 300 * m_level;
				Mix_PlayChannel(3, layers3, 0);
			}
			else if (breakTotal == 4)
			{
				m_score += 1200 * m_level;
				Mix_PlayChannel(3, bigLayers, 0);
			}

			if (breakTotal > 0)
			{
				scoreText->LoadTexture(TTF_RenderText_Blended(m_font, std::string("Score: ").append(std::to_string(m_score)).c_str(), fontColor));
			}

			nextPiece.x = 5;
			nextPiece.y = 0;
			nextPiece.orientation = 0;
			currentPiece = nextPiece;
			nextPiece.shape = (std::rand() % 7) * 16;
		}
	}
}



void Shrektris::draw(float t)
{
	//totalTime += t;

	std::cout << totalTime << std::endl;

	program->Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if (spin)
		program->View->value = glm::lookAt(glm::vec3(0.f, 2.f, 20.f), { 0.f, 2.f, -1.f }, { 0.f, 1.f, 0.f }) * glm::rotate(glm::identity<glm::mat4>(), 6.28f * totalTime, glm::vec3(0.f, 1.f, 0.f));
	else
		program->View->value = glm::lookAt(position, position + target, { 0.f, 1.f, 0.f });
	program->View->ForwardToShader();

	// draw the main game board
	for (int y = 0; y < 15; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (board[y][x] != 0)
			{
				program->Model->value = glm::translate(glm::identity<glm::mat4>(), { x - 5, -(y - 10), 0.f });
				program->Model->ForwardToShader();
				shrek.Draw();
			}
		}
	}

	// draw the next piece
	std::array<int, 16> box;

	auto squares = getSquares(nextPiece);

	for (int i = 0; i < 16; i++)
	{
		box[i] = 0;

		for (int j = 0; j < 4; j++)
		{
			box[squares[j]] = 1;
		}

		if (box[i] != 0)
		{
			program->Model->value = glm::translate(glm::identity<glm::mat4>(), { 6.f + (i % 4), -((i / 4) - 5.f), 0.f });
			program->Model->ForwardToShader();
		}
		shrek.Draw();
	}

	// draw the visualizer

	squares = getSquares(visualizer);

	for (int i = 0; i < 16; i++)
	{
		box[i] = 0;

		for (int j = 0; j < 4; j++)
		{
			box[squares[j]] = 1;
		}

		if (box[i] != 0)
		{
			int x = visualizer.x + (i % 4);
			int y = visualizer.y + (i / 4);
			program->Model->value = glm::translate(glm::identity<glm::mat4>(), { x - 5, -(y - 10), 0.f });
			program->Model->ForwardToShader();
			thanos.Draw();
		}
	}

	spriteProgram->Use();
	glDisable(GL_DEPTH_TEST);
	m_rootNode.Draw();
	m_window->SwapBuffers();
}