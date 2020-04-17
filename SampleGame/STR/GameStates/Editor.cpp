#include "Editor.hpp"

Editor::Editor(std::pair<int, int> size)
{
	glClearColor(0.1f, 0.3f, 0.1f, 1.0f);

	
	{
		cage::Shader genVer(cage::Shader::VERTEX);
		cage::Shader genFrag(cage::Shader::FRAGMENT);

		genVer.CompileFromFile("Assets/generic3D.vert");
		genFrag.CompileFromFile("Assets/generic3D.frag");

		m_genericShader = std::make_shared<cage::Generic3DShader>(genVer, genFrag);

		cage::Shader spriteVer(cage::Shader::VERTEX);
		cage::Shader spriteFrag(cage::Shader::FRAGMENT);

		spriteVer.CompileFromFile("Assets/sprite.vert");
		spriteFrag.CompileFromFile("Assets/sprite.frag");

		m_spriteShader = std::make_shared<cage::SpriteShader>(spriteVer, spriteFrag);
	}

		std::cout << size.first << ", " << size.second << std::endl;

		cage::Texture::MissingTexture = new cage::Texture(IMG_Load("Assets/missing.png"));

		m_genericShader->Use();

		m_genericShader->Projection->value = glm::perspective(glm::quarter_pi<float>(), (float)size.first / size.second, 0.1f, 500.f);
		m_genericShader->Projection->ForwardToShader();

		m_genericShader->Model->value = glm::translate(glm::identity<glm::mat4>(), {5, 5, +5});
		m_genericShader->Model->ForwardToShader();

		shrek.LoadModel("Assets/shrek.obj");

		m_camera = std::make_shared<cage::Camera>(*m_genericShader);

		SDL_SetRelativeMouseMode(SDL_TRUE);

		//auto test = std::make_shared<MenuButton>("test");
		//m_root.Add(test);

}

void Editor::ProcessEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			m_quit = true;
			break;
		case SDL_WINDOWEVENT:
			switch (e.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				auto size = std::make_pair(e.window.data1, e.window.data2);
				glViewport(0, 0, e.window.data1, e.window.data2);
				m_root.Resize({ (float)e.window.data1, (float)e.window.data2 });
				m_root.MoveTo({ (float)e.window.data1 / 2, (float)e.window.data2 / 2 });
				m_spriteShader->Use();
				m_spriteShader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
				m_spriteShader->Projection->ForwardToShader();
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			m_camera->yaw += (float)e.motion.xrel / 10;
			m_camera->pitch -= (float)e.motion.yrel / 10;
			break;
		default:
			m_input.Raise(e);
		}
	}
}

static float tot = 0;
void Editor::Update(float dt)
{
	tot += dt;

	const Uint8* keys = SDL_GetKeyboardState(nullptr);
	const float SPEED = 50.f;
	if (keys[SDL_SCANCODE_A])
	{
		m_camera->MoveLeftRight(-SPEED * dt);
	}
	if (keys[SDL_SCANCODE_D])
	{
		m_camera->MoveLeftRight(SPEED * dt);
	}
	if (keys[SDL_SCANCODE_W])
	{
		m_camera->MoveForward(SPEED * dt);
	}
	if (keys[SDL_SCANCODE_S])
	{
		m_camera->MoveForward(-SPEED * dt);
	}
	if (keys[SDL_SCANCODE_Q])
	{
		m_camera->Move(dt * glm::vec3{ 0.0f,SPEED,0.f });
	}
	if (keys[SDL_SCANCODE_E])
	{
		m_camera->Move(dt * glm::vec3{ 0.f,-SPEED,0.f });
	}
}


void Editor::Draw()
{

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_genericShader->Use();

	m_camera->Apply();

	shrek.Draw();

	glLineWidth(4.0f);
	m_track.GetRoad().Draw(GL_LINE_STRIP);

	glDisable(GL_DEPTH_TEST);
	m_spriteShader->Use();
	m_root.Draw();

}