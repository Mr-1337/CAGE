#include <iostream>
#include <Glad/glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../CAGE/CAGE.hpp"
#include "../CAGE/Graphics/Vertex/Vertex.hpp"
#include "../CAGE/Graphics/VertexArrays/VertexArray.hpp"
#include "../CAGE/Graphics/ShaderProgram/ShaderProgram.hpp"
#include "../CAGE/IO/MeshLoader.hpp"

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

glm::vec3 position = glm::vec3(0, 2, 20), target = glm::vec3(0, 0, -1);
const float speed = 0.04f;

/*
	0 1 2 3
	4 5 6 7
	8 9 A B
	C D E F
*/

int board[15][10] = { 0 };

const int shapes[] = {
	0, 1, 4, 5,    0, 1, 4, 5,    0, 1, 4, 5,   0, 1, 4, 5,   //square
	0, 1, 5, 6,    8, 4, 5, 1,    0, 1, 5, 6,   8, 4, 5, 1,   //z
	4, 5, 1, 2,    0, 4, 5, 9,    4, 5, 1, 2,   0, 4, 5, 9,   //s
	0, 4, 8, 9,    4, 5, 6, 2,    0, 1, 5, 9,   0, 4, 1, 2,   //L
	8, 1, 5, 9,    0, 4, 5, 6,    8, 4, 0, 1,   0, 1, 2, 6,   //backward L
	0, 4, 8, 0xC,  0, 1, 2, 3,    0, 4, 8, 0xC, 0, 1, 2, 3,   //line
	0, 1, 2, 5,    0, 4, 8, 5,    1, 4, 5, 6,   4, 1, 5, 9	  //T
};

struct Tetromino
{
	int x, y;
	int shape = 0;
	int orientation = 0;
};

void input(const Uint8* keyArray)
{
	if (keyArray[SDL_SCANCODE_I])
	{
		position += speed * glm::normalize(glm::vec3(target.x, 0.f, target.z));
	}
	if (keyArray[SDL_SCANCODE_J])
	{
		position -= speed * glm::normalize(glm::cross(target, { 0.f, 1.f, 0.f }));
	}
	if (keyArray[SDL_SCANCODE_K])
	{
		position -= speed * glm::normalize(glm::vec3(target.x, 0.f, target.z));
	}
	if (keyArray[SDL_SCANCODE_L])
	{
		position += speed * glm::normalize(glm::cross(target, { 0.f, 1.f, 0.f }));
	}
	if (keyArray[SDL_SCANCODE_SPACE])
	{
		position.y += speed;
	}
	if (keyArray[SDL_SCANCODE_LCTRL])
	{
		position.y -= speed;
	}
}

Tetromino currentPiece;

int main(int argc, char** argv)
{
	std::cout << sizeof(cage::Vertex3) << std::endl;
	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	auto w = SDL_CreateWindow("Shrektris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(w);

	SDL_assert(context != NULL);

	gladLoadGLLoader(SDL_GL_GetProcAddress);

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));

	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);

	auto& shrek = cage::loadObjVertices("Assets/shrekt.obj");
	//auto& vader = cage::loadObjVertices("shrekt.obj");
	//auto& world = cage::loadObjVertices("shrekt.obj");

	cage::VertexBuffer<cage::Vertex3UVNormal> vbo;

	vbo.Fill(shrek);
	cage::VertexArray<cage::Vertex3UVNormal> vao;


	glViewport(0, 0, 500, 800);

	cage::Shader vertexShader(cage::Shader::VERTEX);
	cage::Shader fragShader(cage::Shader::FRAGMENT);

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

	cage::ShaderProgram program(vertexShader, fragShader);
	program.Use();
	

	program.Projection->value = glm::perspective(glm::quarter_pi<float>(), 5.f/8.f, 0.1f, 100.f);
	program.Projection->ForwardToShader();
	program.Model->value = glm::scale(program.Model->value, { 1.f, 0.6f, 1.f });
	program.Model->ForwardToShader();

	SDL_Event e;
	bool running = true;
	float t = 0;

	glClearColor(0.3f, 0.5f, 0.2f, 0.0f);

	float thetaX = 0, thetaY = 0;
	bool wireframe = false;

	Mix_Chunk *chunk, *chunk2;

	Mix_Chunk* bigLayers = Mix_LoadWAV("Assets/biglayers.ogg");
	Mix_Chunk* layers1 = Mix_LoadWAV("Assets/layers1.ogg");
	Mix_Chunk* layers2 = Mix_LoadWAV("Assets/layers2.ogg");
	Mix_Chunk* donkey = Mix_LoadWAV("Assets/donkey.ogg");

	chunk2 = Mix_LoadWAV("Assets/shrek.ogg");

	//Mix_PlayChannel(1, chunk, -1);
	Mix_PlayChannel(2, chunk2, -1);

	currentPiece.shape = 0;
	currentPiece.y = 1;
	currentPiece.x = 5;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	auto getSquares = [&](const Tetromino & t)
	{
		std::vector<int> squares = { shapes[t.shape + 4 * t.orientation], shapes[t.shape + 4 * t.orientation + 1], shapes[t.shape + 4 * t.orientation + 2], shapes[t.shape + 4 * t.orientation + 3] };
		return squares;
	};

	auto checkCollision = [&](Tetromino & t, const std::vector<int>& squares)
	{
		for (int i = 0; i < 4; i++)
		{
			// check on board first
			if (t.x + squares[i] % 4 > 9 || t.x + squares[i] % 4 < 0)
				return false;
			if (t.y + squares[i] / 4 > 14 || t.y + squares[i] / 4 < 0)
				return false;
			// check if cell is occupied
			if (board[t.y + squares[i] / 4][t.x + squares[i] % 4] != 0)
				return false;
		}
		return true;
	};

	auto movePieceDown = [&](Tetromino & t)
	{
		// indices of all the 'cells' this tetromino fills
		auto squares = getSquares(t);

		for (int i = 0; i < 4; i++)
		{
			board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 0;
		}

		t.y++;

		if (!checkCollision(t, squares))
		{
			t.y--;
			for (int i = 0; i < 4; i++)
			{
				board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 1;
			}
			return false;
		}

		for (int i = 0; i < 4; i++)
		{
			board[t.y + (squares[i] / 4)][t.x + squares[i] % 4] = 1;
		}
		return true;
	};

	auto movePieceLR = [&](Tetromino & t, int dx)
	{
		// indices of all the 'cells' this tetromino fills
		auto squares = getSquares(t);

		for (int i = 0; i < 4; i++)
		{
			board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 0;
		}

		t.x+=dx;

		if (!checkCollision(t, squares))
		{
			t.x-=dx;
			for (int i = 0; i < 4; i++)
			{
				board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 1;
			}
			return false;
		}

		for (int i = 0; i < 4; i++)
		{
			board[t.y + (squares[i] / 4)][t.x + squares[i] % 4] = 1;
		}
		return true;
	};

	auto rotate = [&](Tetromino & t)
	{
		auto squares = getSquares(t);

		for (int i = 0; i < 4; i++)
		{
			board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 0;
		}

		t.orientation++;
		t.orientation %= 4;

		squares = getSquares(t);

		if (!checkCollision(t, squares))
		{
			t.orientation--;
			t.orientation %= 4;
			squares = getSquares(t);
			for (int i = 0; i < 4; i++)
			{
				board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 1;
			}
			return false;
		}

		for (int i = 0; i < 4; i++)
		{
			board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 1;
		}

	};

	int next = 0;

	while (running)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				running = false;
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
			if (e.type == SDL_MOUSEMOTION)
			{
				/*
				thetaX += (float)e.motion.xrel / 150.f;
				thetaY -= (float)e.motion.yrel / 150.f;
				if (thetaY > glm::radians(89.f))
				{
					thetaY = glm::radians(89.f);
				}
				else if (thetaY < glm::radians(-89.f))
				{
					thetaY = glm::radians(-89.f);
				}
				*/
			}
		}

		auto k = SDL_GetKeyboardState(nullptr);
		input(k);


		//target = { glm::cos(thetaY) * glm::cos(thetaX), glm::sin(thetaY), glm::cos(thetaY) * glm::sin(thetaX) };
		//target = glm::normalize(target);

		//program.View->value = glm::lookAt(glm::vec3( 0.f, 2.f, 20.f ), { 0.f, 2.f, -1.f }, { 0.f, 1.f, 0.f }) * glm::rotate(glm::identity<glm::mat4>(), 6.28f * t, glm::vec3(0.f, 1.f, 0.f));
		program.View->value = glm::lookAt(position, position + target, { 0.f, 1.f, 0.f });
		program.View->ForwardToShader();

		t += 1.f/60.f;

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
					return 0;
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
						int *temp = new int[i * 10];
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

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int y = 0; y < 15; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				if (board[y][x] != 0)
				{
					program.Model->value = glm::translate(glm::identity<glm::mat4>(), { x - 5, -(y - 10), 0.f });
					program.Model->ForwardToShader();
					glDrawArrays(GL_TRIANGLES, 0, shrek.size());
				}
			}
		}

		SDL_GL_SwapWindow(w);
	}
	return 0;
}