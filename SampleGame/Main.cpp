#include <iostream>
#include <Glad/glad/glad.h>
#include <SDL2/SDL.h>
#include <GLM/glm/glm.hpp>
#include "../CAGE/CAGE.h"

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

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	auto w = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(w);

	SDL_assert(context != NULL);

	std::cout << gladLoadGLLoader(SDL_GL_GetProcAddress) << std::endl;

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	SDL_GL_SetSwapInterval(1);

	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));

	// Disable depth test and face culling.
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	unsigned int vbo;
	glCreateBuffers(1, &vbo);

	float data[] = {
		-0.5,-0.5,
		0.5,-0.5,
		0,0.5
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STREAM_DRAW);

	unsigned int vao;
	glCreateVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glVertexAttribPointer(0, 2, GL_FLOAT, false, 8, 0);

	glEnableVertexAttribArray(0);


	glViewport(0, 0, 640, 480);

	std::string verShader = R"REE(
	#version 460 core

	layout (location = 0) in vec2 pos;
	out vec2 pos_o;

	void main()
	{
		pos_o = pos;
		gl_Position = vec4(pos.x, pos.y, 0, 1);
	}
	)REE";

	std::cout << verShader.c_str() << std::endl;

	std::string fragShader = R"REE(
	#version 460 core

	in vec2 pos_o;
	out vec4 colorOut;

	void main()
	{
		colorOut = vec4(pos_o.x, pos_o.y, pos_o.x, 1.0) + vec4(1.0, 1.0, 1.0, 0);
		colorOut *= vec4(0.5, 0.5, 0.5, 1.0);
	}
	)REE";

	unsigned int program = glCreateProgram();
	unsigned int ver = glCreateShader(GL_VERTEX_SHADER);
	unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);

	auto vs = verShader.c_str();
	auto fs = fragShader.c_str();
	glShaderSource(ver, 1, &vs, nullptr);
	glShaderSource(frag, 1, &fs, nullptr);
	
	glCompileShader(ver);
	glCompileShader(frag);

	glAttachShader(program, ver);
	glAttachShader(program, frag);

	glLinkProgram(program);

	//Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		printf("Error linking program %d!\n", program);
		if (glIsProgram(program))
		{
			//Program log length
			int infoLogLength = 0;
			int maxLength = infoLogLength;

			//Get info string length
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			//Allocate string
			char* infoLog = new char[maxLength];

			//Get info log
			glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
			if (infoLogLength > 0)
			{
				//Print Log
				printf("%s\n", infoLog);
			}

			//Deallocate string
			delete[] infoLog;
		}
		else
		{
			printf("Name %d is not a program\n", program);
		}
	}

	glUseProgram(program);

	glBindVertexArray(vao);

	SDL_Event e;
	bool running = true;
	float t = 0;
	
	const float data2[] = {
	-0.5,-0.5,
	0.5,-0.5,
	0,0.5
	};

	while (running)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				running = false;
			}
		}
		for (int i = 0; i < 6; i++)
		{
			if (i % 2 == 0)
			{
				//data[i] = data2[i] + glm::sin(t / 2) / 3.0f;
				data[i] = data2[i] * glm::cos(glm::atan(data[i+1]/data[i]) + t / 5);
			}
			else
			{
				data[i] = -data2[i] * glm::sin(glm::atan(data[i] / data[i-1]) + t / 5);
			}
		}
		//SDL_SetWindowOpacity(w, 0.5f* (glm::sin(SDL_GetTicks() / 5000.f) + 1.0));
		t += 0.1;
		//SDL_SetWindowPosition(w, 400 + 200.f * sin(t), 300 + 100.f* cos(t));
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
		SDL_GL_SwapWindow(w);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	return 0;
}