#include "Shrektris.hpp"
#include <Glad/glad/glad.h>

Shrektris::Shrektris(int argc, char** argv) : 
	Game("Shrektris", argc, argv),
	m_running(true), 
	vertexShader(cage::Shader::VERTEX), 
	fragShader(cage::Shader::FRAGMENT),
	shrek("Shrektromino!")
{
	glViewport(0, 0, 500, 800);

	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);

	shrek.SetGeometry(cage::LoadObjVertices("Assets/shrek.obj"));

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
		uv_o = uv;
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

	void main()
	{
		float diffuse = dot(norm_o, normalize(-pos_world_o + vec3(0, 100, 0)));
		colorOut = vec4(diffuse, diffuse, diffuse, 1.0);
	}
	)REE";

	vertexShader.CompileFromSrcString(vsString);
	fragShader.CompileFromSrcString(fsString);

	program = std::make_unique<cage::ShaderProgram>(vertexShader, fragShader);

	program->Use();

	program->Projection->value = glm::perspective(glm::quarter_pi<float>(), 5.f / 8.f, 0.1f, 100.f);
	program->Projection->ForwardToShader();
	program->Model->value = glm::scale(program->Model->value, { 1.f, 0.6f, 1.f });
	program->Model->ForwardToShader();

	music = Mix_LoadWAV("Assets/shrek.ogg");
	bigLayers = Mix_LoadWAV("Assets/biglayers.ogg");
	layers1 = Mix_LoadWAV("Assets/layers1.ogg");
	layers2 = Mix_LoadWAV("Assets/layers2.ogg");
	donkey = Mix_LoadWAV("Assets/donkey.ogg");

	Mix_PlayChannel(2, music, -1);

	currentPiece.shape = 0;
	currentPiece.y = 1;
	currentPiece.x = 5;

	glClearColor(0.3f, 0.5f, 0.2f, 0.0f);
}

void Shrektris::Run()
{
	SDL_Event e;
	bool wireframe = false;
	float t = 0;
	while (m_running)
	{
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
					movePieceDown(currentPiece);
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_E)
				{
					rotate(currentPiece);
				}
			}
		}
		t += 1.f / 60.f;
		logic(t);
		draw();
	}
}

void Shrektris::logic(float& t)
{
	if (t >= 1.f)
	{
		t = 0.f;
		std::cout << "MOVE" << std::endl;
		if (!movePieceDown(currentPiece))
		{
			if (currentPiece.y == 0)
			{
				Mix_HaltChannel(2);
				Mix_PlayChannel(4, donkey, 0);
				SDL_Delay(7000);
				m_running = false;
				return;
			}
			std::cout << "FAILURE!!!" << std::endl;
			SDL_Delay(500);

			//Clear any rows we should

			int breakTotal = 0;
			for (size_t i = 15; i > 0; i--)
			{

				int yesCount = 0;
				for (int j = 0; j < 10; j++)
				{
					if (board[i][j] != 0)
						yesCount++;
				}
				if (yesCount == 10)
				{
					for (int k = 0; k < 10; k++)
						board[i][k] = 0;
					int* temp = new int[i * 10];
					memcpy(temp, board, (i * 10 * sizeof(int)));
					memset(board, 0, (i * 10 * sizeof(int)));
					memcpy(&board[1][0], temp, i * 10 * sizeof(int));
					delete[] temp;
					i++;
					breakTotal++;
				}

			}

			if (breakTotal == 1)
			{
				Mix_PlayChannel(3, layers1, 0);
			}
			else if (breakTotal == 2 || breakTotal == 3)
			{
				Mix_PlayChannel(3, layers2, 0);
			}
			else if (breakTotal == 4)
			{
				Mix_PlayChannel(3, bigLayers, 0);
			}

			currentPiece.x = 5;
			currentPiece.y = 0;
			currentPiece.shape = (std::rand() % 7) * 16;
		}
	}
}

void Shrektris::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//program->View->value = glm::lookAt(glm::vec3( 0.f, 2.f, 20.f ), { 0.f, 2.f, -1.f }, { 0.f, 1.f, 0.f }) * glm::rotate(glm::identity<glm::mat4>(), 6.28f * t, glm::vec3(0.f, 1.f, 0.f));
	program->View->value = glm::lookAt(position, position + target, { 0.f, 1.f, 0.f });
	program->View->ForwardToShader();

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

	m_window->SwapBuffers();
}