#include <GLM/glm/glm.hpp>
#include <GLM/glm/matrix.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_ttf.h>
#include "STR.hpp"
#include "Perlin.hpp"

std::shared_ptr<cage::ui::UIElement> child, child2, child3;

STR::STR(int argc, char** argv) : 
	Game("STR", argc, argv), 
	m_running(true), 
	verShader(cage::Shader::VERTEX), 
	fragShader(cage::Shader::FRAGMENT), 
	spriteVerShader(cage::Shader::VERTEX),
	spriteFragShader(cage::Shader::FRAGMENT),
	world("World", true)
{
	auto size = m_window->GetSize();
	glViewport(0, 0, size.first, size.second);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.8f, 0.5f, 0.1f, 1.0f);
	//glClearColor(0.3f, 0.5f, 0.2f, 0.0f);

	world.BindVAO();
	world.SetGeometry(genTerrain());
	TTF_Font* font = TTF_OpenFont("Assets/sans.ttf", 32);
	SDL_Color color;
	color.g = 200;
	color.r = 000;
	color.b = 100;
	color.a = 255;
	auto surface = TTF_RenderText_Blended(font, "Coming soon to a swamp near you!", color);
	auto surface2 = TTF_RenderText_Blended(font, "I hope that odor isn't coming from you", color);
	shrek.LoadModel("Assets/shrek.obj");
	controller = SDL_GameControllerOpen(0);
	//SDL_assert(controller != nullptr);

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

		void main()
		{
			float diffuse = max(dot(norm_o, normalize(-pos_world_o + vec3(30.0, 100.0, 30.0))), 0.1);
			//colorOut = vec4(norm_o.r, norm_o.g, norm_o.b, 1.0);

			colorOut = vec4(diffuse, diffuse, diffuse, 1.0) * texture(u_texture, uv_o);
		}
		)REE";

	verShader.CompileFromSrcString(vsString);
	fragShader.CompileFromSrcString(fsString);

	vsString = R"REE(
		#version 460 core

		layout (location = 0) in vec2 pos;

		uniform mat4 u_model;
		uniform mat4 u_projection;
		uniform vec2 u_spriteSize;

		out vec2 uv_o;

		void main()
		{
			uv_o = vec2(pos.x + 0.5, 0.5 + pos.y);
			gl_Position = u_projection * u_model * (vec4(u_spriteSize, 0, 1.0) * vec4(pos, 0, 1.0));
		}
		)REE",
		fsString = R"REE(
		#version 460 core

		in vec2 uv_o;

		uniform sampler2D u_texture;

		out vec4 colorOut;

		void main()
		{
			colorOut += texture(u_texture, uv_o);
		}
		)REE";

	program = new cage::Generic3DShader(verShader, fragShader);

	program->Use();

	camera = new cage::Camera(*program);
	//camera->SetProjection(glm::ortho(-1.f, 1.f, -1.f, 1.f, 0.1f, 100.f));
	camera->SetProjection(glm::perspective(glm::quarter_pi<float>(), (float)size.first / size.second, 0.1f, 5000.f));
	camera->SetPosition({ 0.f, 1.f, 0.f });

	spriteVerShader.CompileFromSrcString(vsString);
	spriteFragShader.CompileFromSrcString(fsString);

	spriteProgram = new cage::SpriteShader(spriteVerShader, spriteFragShader);
	spriteProgram->Use();
	//spriteProgram->Projection->value = glm::identity<glm::mat4>();
	spriteProgram->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
	spriteProgram->Projection->ForwardToShader();

	auto surface3 = IMG_Load("Assets/simon.png");
	m_root.LoadTexture(surface);
	m_root.MoveTo(glm::vec2{ 50.f });
	m_root.Resize({ 1.f, 1.f });

	cage::ui::UIElement::shader = spriteProgram;

	child = std::make_shared<cage::ui::UIElement>();
	child2 = std::make_shared<cage::ui::UIElement>();
	child3 = std::make_shared<cage::ui::UIElement>();

	child->LoadTexture(surface);
	child2->LoadTexture(surface);
	child3->LoadTexture(surface);

	child->Scale(0.5f);
	child2->Scale(0.3f);

	child2->Add(child3);
	child->Add(child2);

	m_root.Add(child);
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
		camera->MoveLeftRight(-0.5f);
	}
	if (keys[SDL_SCANCODE_D])
	{
		camera->MoveLeftRight(0.5f);
	}
	if (keys[SDL_SCANCODE_W])
	{
		camera->MoveForward(0.5f);
	}
	if (keys[SDL_SCANCODE_S])
	{
		camera->MoveForward(-0.5f);
	}
	if (keys[SDL_SCANCODE_Q])
	{
		camera->Move({ 0.0f,-0.5f,0.f });
	}
	if (keys[SDL_SCANCODE_E])
	{
		camera->Move({ 0.f,0.5f,0.f });
	}
	if (keys[SDL_SCANCODE_SPACE])
	{
		i = 0;
	}

	m_root.MoveTo(glm::vec2{ 70.f * cosf(i/10) + 70, 70.f * sinf(i/10) + 70 });
	m_root.Resize({ sinf((float)i / 10.f), sinf((float)i / 10.f) });
	child->MoveTo(glm::vec2{ 15.f * cosf(i), 15.f * sinf(i) });
	child2->MoveTo(glm::vec2{ 5.f * sinf(i / 18), 5.f * cosf(i / 26) });
	child3->MoveTo(glm::vec2{ 25.f * cosf(i / 2), 25.f * sinf(i / 2) });

	short controllerY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
	short controllerX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
	if (-1000 < controllerY && controllerY < 1000)
		controllerY = 0;
	if (-1000 < controllerX && controllerX < 1000)
		controllerX = 0;
	camera->MoveForward(-(float)controllerY / 65535.f / 5);
	camera->MoveLeftRight((float)controllerX / 65535.f / 5);
	controllerY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
	controllerX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
	if (-1000 < controllerY && controllerY < 1000)
		controllerY = 0;
	if (-1000 < controllerX && controllerX < 1000)
		controllerX = 0;
	camera->pitch -= (float)controllerY / 65535.f * 10.f;
	camera->yaw += (float)controllerX / 65535.f * 10.f;

	//std::cout << SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) << std::endl;
	//camera->Move({ 0.0f, -0.2f, 0.0f });
	auto v = camera->GetPosition() + glm::vec3{ 0.0f, -0.7f, 3.0f };
	//std::cout << v.x << ", " << v.y << ", " << v.z << std::endl;
	int xIndex = std::floorf(v.x / 5.f), zIndex = std::floorf(v.z / 5.f);
	float dx = v.x / 5.f - xIndex;
	float dz = v.z / 5.f - zIndex;
	//std::cout << dx << ", " << dz << std::endl;
	float yInterp1 = (1.0f - dx) * world.GetBuffer()[xIndex + (100 * zIndex)].position.y +     dx * world.GetBuffer()[xIndex + 1 + (100 * zIndex)].position.y;
	float yInterp2 = (1.0f - dx) * world.GetBuffer()[xIndex + (100 * (zIndex+1))].position.y + dx * world.GetBuffer()[xIndex + 1 + (100 * (zIndex + 1))].position.y;
	float yInterp = dz * yInterp2 + (1.0f - dz) * yInterp1;
	//std::cout << yInterp1 << ", " << yInterp2 << ", " << yInterp << std::endl;
	auto diff = v.y - yInterp;
	if (diff < 0)
	{
		camera->Move({ 0.0f, -diff, 0.0f });
	}
	m_root.Rotate(0.04f);
	child->Rotate(0.03f);
	child2->Rotate(0.02f);
	child3->Rotate(0.01f);
}



void STR::draw()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	program->Use();
	camera->Apply();
	program->Model->value = glm::translate(glm::identity<glm::mat4>(), camera->GetPosition() + glm::vec3{0.0f, -0.7f, 3.0f});
	program->Model->ForwardToShader();
	shrek.Draw();
	i += 0.1f;
	program->Model->value = glm::identity<glm::mat4>();
	program->Model->ForwardToShader();
	world.DrawIndexed(100 * 100 * 6);

	glDisable(GL_DEPTH_TEST);
	spriteProgram->Use();

	m_root.Draw();

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
			float temp = Perlin::OctavePerlin3((float)x / 70.f, (float)y / 70.f, 4.f, 3, 0.5) * heightScale * 12;
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
			float dz = positions[(y+1) * size + x].y - positions[(y-1) * size + x].y;

			normals[y * size + x] = glm::normalize(glm::vec3{-dx * 0.5f, 1.f, -dz * 0.5f});
		}
	}
	// Construct final geometry
	for (int i = 0; i < size * size; i++)
	{
			vertices.emplace_back(positions[i], glm::vec2{ 1.0 - (float)(i%size)/size, (float)(i/size)/size*2}, normals[i]);
	}

	glCreateBuffers(1, &eboid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	return vertices;
}