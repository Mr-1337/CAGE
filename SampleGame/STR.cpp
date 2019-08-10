#include <GLM/glm/glm.hpp>
#include <GLM/glm/matrix.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include "STR.hpp"
#include "Perlin.hpp"

STR::STR(int argc, char** argv) : Game("STR", argc, argv), m_running(true), us(25565), verShader(cage::Shader::VERTEX), fragShader(cage::Shader::FRAGMENT), worldvbo(true)
{
	auto size = m_window->GetSize();
	glViewport(0, 0, size.first, size.second);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.8f, 0.5f, 0.1f, 1.0f);
	IPaddress ip;
	SDLNet_ResolveHost(&ip, "88.156.244.174", 25565);
	them = new cage::networking::Endpoint(ip);
	shrek = cage::LoadObjVertices("Assets/shrek.obj");

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

	vbo.Fill(shrek);
	vao = new cage::VertexArray<cage::Vertex3UVNormal>();
	vbo.Bind();

	worldvbo.Bind();
	vao2 = new cage::VertexArray<cage::Vertex3UVNormal>();
	worldvbo.Fill(genTerrain());

	verShader.CompileFromSrcString(vsString);
	fragShader.CompileFromSrcString(fsString);

	program = new cage::ShaderProgram(verShader, fragShader);

	program->Use();

	camera = new cage::Camera(*program);
	//camera->SetProjection(glm::ortho(-1.f, 1.f, -1.f, 1.f, 0.1f, 100.f));
	camera->SetProjection(glm::perspective(glm::quarter_pi<float>(), (float)size.first / size.second, 0.1f, 5000.f));
	camera->SetPosition({ 0.f, 1.f, 0.f });
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

	SDL_SetRelativeMouseMode(SDL_TRUE);

	while (m_running)
	{
		processEvents(e);
		update(deltaTime);
		draw();

		deltaTime = (float)(clock.now() - lastTime).count() / 1000000000.0f;
		//SDL_Delay(25);
	}
}

void STR::processEvents(SDL_Event& e)
{
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			m_running = false;
			break;
		case SDL_KEYDOWN:
			if (!e.key.repeat)
			{
				if (e.key.keysym.sym == SDLK_f)
					SDL_SetRelativeMouseMode((SDL_bool)!SDL_GetRelativeMouseMode());
				else if (e.key.keysym.sym == SDLK_g)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else if (e.key.keysym.sym == SDLK_h)
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				else if (e.key.keysym.sym == SDLK_u)
					camera->ToggleMode();
			}
			break;
		case SDL_MOUSEMOTION:
			camera->yaw += (float)e.motion.xrel / 10;
			camera->pitch -= (float)e.motion.yrel / 10;
			break;
		}
	}
}

static float i = 0.f;

void STR::update(float delta)
{
	const Uint8* keys = SDL_GetKeyboardState(nullptr);
	if (keys[SDL_SCANCODE_A])
	{
		camera->MoveLeftRight(-0.1f);
	}
	if (keys[SDL_SCANCODE_D])
	{
		camera->MoveLeftRight(0.1f);
	}
	if (keys[SDL_SCANCODE_W])
	{
		camera->MoveForward(0.1f);
	}
	if (keys[SDL_SCANCODE_S])
	{
		camera->MoveForward(-0.1f);
	}
	if (keys[SDL_SCANCODE_Q])
	{
		camera->Move({ 0.0f,-0.1f,0.f });
	}
	if (keys[SDL_SCANCODE_E])
	{
		camera->Move({ 0.f,0.1f,0.f });
	}
	if (keys[SDL_SCANCODE_SPACE])
	{
		i = 0;
	}
	//camera->Move({ 0.0f, -0.2f, 0.0f });
	auto v = camera->GetPosition() + glm::vec3{ 0.0f, -0.7f, 3.0f };
	std::cout << v.x << ", " << v.z << std::endl;
	int xIndex = std::floorf(v.x / 5.f), zIndex = std::floorf(v.z / 5.f);
	float dx = v.x / 5.f - xIndex;
	float dz = v.z / 5.f - zIndex;
	std::cout << dx << ", " << dz << std::endl;
	float yInterp1 = (1.0f - dx) * worldvbo[xIndex + (100 * zIndex)].position.y +     dx * worldvbo[xIndex + 1 + (100 * zIndex)].position.y;
	float yInterp2 = (1.0f - dx) * worldvbo[xIndex + (100 * (zIndex+1))].position.y + dx * worldvbo[xIndex + 1 + (100 * (zIndex + 1))].position.y;
	float yInterp = dz * yInterp2 + (1.0f - dz) * yInterp1;
	std::cout << yInterp1 << ", " << yInterp2 << ", " << yInterp << std::endl;
	auto diff = v.y - yInterp;
	if (diff < 0)
	{
		camera->Move({ 0.0f, -diff, 0.0f });
	}
}



void STR::draw()
{
	vao->Bind();
	vbo.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->Apply();
	program->Model->value = glm::translate(glm::identity<glm::mat4>(), camera->GetPosition() + glm::vec3{0.0f, -0.7f, 3.0f});
	program->Model->ForwardToShader();
	glDrawArrays(GL_TRIANGLES, 0, shrek.size());
	vao2->Bind();
	i += 0.1f;
	program->Model->value = glm::identity<glm::mat4>();
	program->Model->ForwardToShader();
	glDrawElements(GL_TRIANGLES, 100 * 100 * 6, GL_UNSIGNED_INT, 0);
	m_window->SwapBuffers();
}

unsigned int eboid;

std::vector<cage::Vertex3UVNormal> STR::genTerrain()
{
	long size = 100;
	std::vector<cage::Vertex3UVNormal> vertices;
	std::vector<glm::vec3> positions, normals;
	std::vector<unsigned int> indices;
	vertices.reserve(size * size);
	normals.resize(size * size);
	positions.reserve(size * size);
	float height = 0.f;
	const float scale = 5.f;
	const float heightScale = 10.f;
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			height = 0;
			float temp = Perlin::OctavePerlin3((float)x / 70.f, (float)y / 70.f, 4.f, 8, 0.5) * heightScale * 12;
			//if (temp > 0.5 * heightScale * 12)
				height = temp;
			//height += Perlin::OctavePerlin3((float)x/15.f, (float)y/15.f, 4.f, 1, 0.2) * heightScale;
			//height += Perlin::OctavePerlin3((float)x / 5.f, (float)y / 5.f, 4.f, 1, 0.2) * heightScale * 0.5;
			//height = (float)std::rand() / RAND_MAX;
			positions.emplace_back(x * scale, height, y * scale);

			if (x < size - 1 && y < size - 1)
			{
				indices.push_back(y * size + x);
				indices.push_back(y * size + x + 1);
				indices.push_back((y + 1) * size + x);

				indices.push_back(y * size + x + 1);
				indices.push_back((y + 1) * size + x);
				indices.push_back((y + 1) * size + x + 1);
			}

		}
	}


	// Calculate normals
	for (long y = 1; y < size - 1; y++)
	{
		for (long x = 1; x < size - 1; x++)
		{
			float dx = positions[y * size + x + 1].y - positions[y * size + x - 1].y;
			float dy = positions[(y+1) * size + x].y - positions[(y-1) * size + x].y;

			normals[y * size + x] = glm::normalize(glm::vec3{dx, dy, -2.f});
		}
	}
	// Construct final geometry
	for (int i = 0; i < size * size; i++)
	{
			vertices.emplace_back(positions[i], glm::vec2{ 0.f, 0.f }, normals[i]);
	}

	vao2->Bind();
	worldvbo.Bind();
	glCreateBuffers(1, &eboid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	return vertices;
}