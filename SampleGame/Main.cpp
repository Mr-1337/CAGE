#include <iostream>
#include <Glad/glad/glad.h>
#include <SDL2/SDL.h>
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../CAGE/CAGE.hpp"
#include "../CAGE/Graphics/Vertex/Vertex.hpp"
#include "../CAGE/Graphics/VertexArrays/VertexArray.hpp"
#include "../CAGE/Graphics/ShaderProgram/ShaderProgram.hpp"

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
	std::cout << sizeof(cage::Vertex3) << std::endl;
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

	cage::VertexBuffer<cage::Vertex3> vbo;

	std::vector<cage::Vertex3> data = {
		{-0.5f, -0.5f, 0.5f },
		{ 0.5f, -0.5f, 0.5f },
		{ 0.0f,  0.366f, 0.5f }
	};

	vbo.Fill(data);

	cage::VertexArray<cage::Vertex3> vao;

	glViewport(0, 0, 640, 480);

	cage::Shader vertexShader(cage::Shader::VERTEX);
	cage::Shader fragShader(cage::Shader::FRAGMENT);

	std::string vsString = R"REE(
	#version 460 core

	layout (location = 0) in vec3 pos;

	uniform mat4 u_projection;
	uniform mat4 u_model;

	out vec2 pos_o;

	void main()
	{
		pos_o = (u_projection * u_model * vec4(pos, 1.0)).xy;
		gl_Position = vec4(pos_o, 0, 1);
	}
	)REE";

	std::string fsString = R"REE(
	#version 460 core

	in vec2 pos_o;
	out vec4 colorOut;

	void main()
	{
		colorOut = vec4(pos_o.x, pos_o.y, pos_o.x, 1.0) + vec4(1.0, 1.0, 1.0, 0);
		colorOut *= vec4(0.7, 0.7, 0.7, 1.0);
	}
	)REE";

	vertexShader.CompileFromSrcString(vsString);
	fragShader.CompileFromSrcString(fsString);

	cage::ShaderProgram program(vertexShader, fragShader);
	program.Use();
	

	program.Projection->value = glm::ortho(-2.f, 2.f, -1.5f, 1.5f, 0.1f, 100.0f);
	program.Projection->ForwardToShader();

	vao.Bind();

	SDL_Event e;
	bool running = true;
	float t = 0;
	
	const float data2[] = {
		-0.5,-0.5,
		 0.5,-0.5,
		 0.0, 0.366
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

		//SDL_SetWindowOpacity(w, 0.5f* (glm::sin(SDL_GetTicks() / 5000.f) + 1.0));
		t += 0.03;

		program.Model->value = glm::rotate(glm::identity<glm::mat4>(), t, glm::vec3(0, 0, 1));
		program.Model->ForwardToShader();
		//SDL_SetWindowPosition(w, 400 + 200.f * sin(t), 300 + 100.f* cos(t));
		
		SDL_GL_SwapWindow(w);
		glClearColor(0.3f, 0.5f, 0.2f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	return 0;
}