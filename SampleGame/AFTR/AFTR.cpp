#include <chrono>
#include <iostream>
#include <Glad/glad/glad.h>
#include "AFTR.hpp"

AFTR::AFTR(int argc, char** argv) : cage::Game("AFTR", argc, argv), m_network(WORLD_SIZE), m_orientation(aftr::Directions::UP)
{
	m_running = true;
	world.resize(WORLD_SIZE * WORLD_SIZE * WORLD_SIZE, nullptr);
	auto size = m_window->GetSize();
	glViewport(0, 0, size.first, size.second);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glClearColor(0.005f, 0.0f, 0.1f, 1.0f);
	glClearColor(1.0, 0, 1.0, 1.0);

	m_cubePosition = { 0.f };

	SDL_SetRelativeMouseMode(SDL_TRUE);

	{
		cage::Shader vertexShader(cage::Shader::VERTEX), fragmentShader(cage::Shader::FRAGMENT);

		// THIS ONLY WORKS IN VISUAL STUDIO :(
		#pragma region StandardShaderSrc
		std::string
			vsString = R"REE(
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
			uv_o = vec2(uv.x, 1.0 - uv.y);
			gl_Position = pos_o;
		}
		)REE",
			fsString = R"REE(
		#version 460 core

		in vec4 pos_o;
		in vec3 pos_world_o;
		in vec2 uv_o;
		in vec3 norm_o;
		out vec4 colorOut;

		uniform sampler2D u_texture;
		uniform int u_mode;

		void main()
		{
			float diffuse = max(dot(norm_o, normalize(-pos_world_o + vec3(30.0, 100.0, 30.0))), 0.1);
			switch(u_mode)
			{
			case 0:
				colorOut = vec4(norm_o.r, norm_o.g, norm_o.b, 1.0);
				break;
			case 1:
				colorOut = vec4(1.0, 0.0, 0.0, 1.0);
				break;
			case 2:
				colorOut = vec4(0.0, 0.0, 1.0, 1.0);
				break;
			}
			colorOut *= diffuse;
			colorOut.a = 1.0;
			//colorOut = vec4(diffuse, diffuse, diffuse, 1.0) * texture(u_texture, uv_o);
		}
		)REE";
#pragma endregion

		vertexShader.CompileFromSrcString(vsString);
		fragmentShader.CompileFromSrcString(fsString);

		m_shader = std::make_shared<aftr::PartShader>(vertexShader, fragmentShader);
	}

	m_shader->Use();
	m_camera = std::make_unique<cage::Camera>(*m_shader);
	m_camera->SetPosition({ 0.f, 0.f, -3.f });
	m_camera->IgnoreYaw(true);

	cage::Texture::MissingTexture = new cage::Texture(IMG_Load("Assets/missing.png"));

	m_camera->SetProjection(glm::perspective(glm::quarter_pi<float>(), static_cast<float>(size.first) / size.second, 0.1f, 100.f));
	
	m_shader->Model->value = glm::identity<glm::mat4>();
	m_shader->Model->ForwardToShader();

	m_tube.LoadModel("Assets/tube.obj");
	m_cross.LoadModel("Assets/cross.obj");
	m_cube.LoadModel("Assets/cube.obj");
	m_inhibitor.LoadModel("Assets/flux inhibitor.obj");
	m_shrek.LoadModel("Assets/shrek.obj");

	m_inhibitor.model = m_shader->Model;

	m_parts[0] = &m_tube;
	m_parts[1] = &m_cross;
	m_parts[2] = &m_inhibitor;
	m_parts[3] = &m_cube;
	m_parts[4] = &m_shrek;
}

AFTR::~AFTR()
{

}

double totalTime = 0;

void AFTR::Run()
{
	using namespace std::chrono;

	using GameClock = high_resolution_clock;
	time_point<steady_clock> last;
	double dt = 1.0 / 60.0;
	while (m_running)
	{
		handleEvents();
		update(dt);
		draw();

		duration<double> elapsedTime = GameClock::now() - last;
		dt = elapsedTime.count();
		totalTime += dt;
		last = GameClock::now();
	}
}

void AFTR::update(double dt)
{
	auto keys = SDL_GetKeyboardState(nullptr);

	const double speed = 8.0;
	const double distance = speed * dt;

	if (keys[SDL_SCANCODE_A])
	{
		m_camera->MoveLeftRight(-distance);
	}
	if (keys[SDL_SCANCODE_D])
	{
		m_camera->MoveLeftRight(distance);
	}
	if (keys[SDL_SCANCODE_W])
	{
		m_camera->MoveForward(distance);
	}
	if (keys[SDL_SCANCODE_S])
	{
		m_camera->MoveForward(-distance);
	}
	if (keys[SDL_SCANCODE_LSHIFT])
	{
		m_camera->Move({ 0.0f,-distance,0.f });
	}
	if (keys[SDL_SCANCODE_SPACE])
	{
		m_camera->Move({ 0.f,distance,0.f });
	}
	if (keys[SDL_SCANCODE_1])
	{
		m_orientation = aftr::Directions::UP;
	}
	if (keys[SDL_SCANCODE_2])
	{
		m_orientation = aftr::Directions::NORTH;
	}
	if (keys[SDL_SCANCODE_3])
	{
		m_orientation = aftr::Directions::EAST;
	}

	static float t = 0;
	t += dt;

	if (t > 3)
	{
		std::cout << m_cubePosition.x << ", " << m_cubePosition.y << ", " << m_cubePosition.z << '\n';
		t = 0;
	}

	placeSpot = m_camera->GetPosition() + glm::vec3(2.5f * m_camera->GetFront());
	int x = (int)std::round(placeSpot.x), y = (int)std::round(placeSpot.y), z = (int)std::round(placeSpot.z);
	m_cubePosition = glm::vec3(x, y, z);

}

void AFTR::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_shader->Use();
	m_camera->Apply();
	drawGrid();


	glm::vec4 rotation = oriToRot(m_orientation);
	m_shader->Model->value = glm::rotate(glm::translate(glm::identity<glm::mat4>(), m_cubePosition), rotation.w, glm::vec3(rotation));
	m_shader->Model->ForwardToShader();
	m_shader->Mode->value = 0;
	m_shader->Mode->ForwardToShader();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_parts[m_selection]->Draw();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_window->SwapBuffers();
}

void AFTR::drawGrid()
{
	for (int i = 0; i < world.size(); i++)
	{
		if (world[i] != nullptr)
		{
			glm::vec3 offset;
			glm::vec4 rotation;

			offset.z = i / (WORLD_SIZE * WORLD_SIZE);
			offset.y = (i / WORLD_SIZE) % WORLD_SIZE;
			offset.x = i % WORLD_SIZE;

			rotation = oriToRot(m_network.GetNode(i)->GetOrientation());

			m_shader->Model->value = glm::rotate(glm::translate(glm::identity<glm::mat4>(), offset), rotation.w, glm::vec3(rotation));
			m_shader->Model->ForwardToShader();
			m_shader->Mode->value = m_network.GetNode(i)->GetState() + 1;
			m_shader->Mode->ForwardToShader();
			if (world[i] == &m_inhibitor)
				((Inhibitor*)(world[i]))->SetTime(totalTime + i);
			world[i]->Draw();
		}
	}
}

glm::vec4 AFTR::oriToRot(aftr::Directions orientation)
{
	float pi = glm::pi<float>();
	switch (orientation)
	{
	case aftr::Directions::UP:
		return { 1, 0.f, 0.f, 0.f };
	case aftr::Directions::DOWN:
		return { 1, 0.f , 0.f, pi };
	case aftr::Directions::NORTH:
		return { 1, 0.f, 0.f, pi/2 };
	case aftr::Directions::SOUTH:
		return { 1, 0.f, 0.f, -pi / 2 };
	case aftr::Directions::EAST:
		return { 0.f, 0.f, 1, pi / 2 };
	case aftr::Directions::WEST:
		return { 0.f, 0.f, 1, -pi / 2 };
	default:
		return { 1, 0.f, 0.f ,0.f };
	}
}

aftr::ComponentBase* AFTR::partFactory(int partID)
{
	void* p = nullptr;
	switch (partID)
	{
	case 0:
		p = new aftr::Pipe(m_orientation);
		break;
	case 1:
		p = new aftr::Cross();
		break;
	case 2:
		p = new aftr::Well();
		break;
	case 3:
		break;
	}
	return (aftr::ComponentBase*)p;
}

void AFTR::handleEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			m_running = false;
			break;
		case SDL_MOUSEMOTION:
			m_camera->yaw += (float)e.motion.xrel / 10;
			m_camera->pitch -= (float)e.motion.yrel / 10;
			break;
		case SDL_KEYDOWN:
			if (!e.key.repeat)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_TAB:
					SDL_SetRelativeMouseMode((SDL_bool)!SDL_GetRelativeMouseMode());
					break;
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (e.button.button == SDL_BUTTON_LEFT)
			{
				if (placeSpot.x >= 0 && placeSpot.y >= 0 && placeSpot.z >= 0)
				{
					int x = (int)std::round(placeSpot.x), y = (int)std::round(placeSpot.y), z = (int)std::round(placeSpot.z);
					aftr::ComponentBase* newNode = partFactory(m_selection);
					bool result = m_network.Add(newNode, x, y, z);
					if (result)
						world[z * WORLD_SIZE * WORLD_SIZE + y * WORLD_SIZE + x] = m_parts[m_selection];
				}
			}
			else
			{
				if (placeSpot.x >= 0 && placeSpot.y >= 0 && placeSpot.z >= 0)
				{
					int x = (int)std::round(placeSpot.x), y = (int)std::round(placeSpot.y), z = (int)std::round(placeSpot.z);
					world[z * WORLD_SIZE * WORLD_SIZE + y * WORLD_SIZE + x] = nullptr;
					m_network.Remove(x, y, z);
				}
			}
			break;
		case SDL_MOUSEWHEEL:
			m_selection += e.wheel.y;
			m_selection %= (int)m_parts.size();
			if (m_selection < 0)
				m_selection = m_parts.size() + m_selection;
			break;
		}
	}
}