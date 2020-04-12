#include "Sandbox.hpp"

Sandbox::Sandbox(std::pair<int, int> size) :
	verShader(cage::Shader::ShaderType::VERTEX),
	fragShader(cage::Shader::ShaderType::FRAGMENT),
	spriteVerShader(cage::Shader::ShaderType::VERTEX),
	spriteFragShader(cage::Shader::ShaderType::FRAGMENT),
	world("World", true)
{
	world.BindVAO();
	world.SetGeometry(genTerrain());
	//glClearColor(0.4f, 0.5f, 0.2f, 0.0f);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	shrek.LoadModel("Assets/shrek.obj");
	thanos.LoadModel("Assets/thanos.obj");
	//vader.LoadModel("Assets/vader.obj");

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
			//float diffuse = max(dot(norm_o, normalize(-pos_world_o + vec3(200.0, 20.0, 200.0))) / (0.05 * length(-pos_world_o + vec3(200.0, 20.0, 200.0))), 0.001);
			float diffuse = max(dot(norm_o, normalize(-pos_world_o + vec3(200.0, 20.0, 200.0))), 0.1);
			//colorOut = vec4(norm_o.r, norm_o.g, norm_o.b, 1.0);

			colorOut = vec4(diffuse, diffuse * 0.6, diffuse * 0.7, 1.0) * texture(u_texture, uv_o);
		}
		)REE";

	verShader.CompileFromSrcString(vsString);
	fragShader.CompileFromSrcString(fsString);

	m_font = TTF_OpenFont("Assets/sans.ttf", 36);
	music = Mix_LoadWAV("Assets/thanos.ogg");

	program = new cage::Generic3DShader(verShader, fragShader);

	program->Use();

	camera = new cage::Camera(*program);
	//camera->SetProjection(glm::ortho(-1.f, 1.f, -1.f, 1.f, 0.1f, 100.f));
	camera->SetProjection(glm::perspective(glm::quarter_pi<float>(), (float)size.first / size.second, 0.1f, 5000.f));
	camera->SetPosition({ 0.f, 1.f, 0.f });

	cage::Texture::MissingTexture = new cage::Texture(IMG_Load("Assets/missing.png"));
	m_speedometer = std::make_shared<cage::ui::UIElement>();
	m_speedometer->LoadTexture(IMG_Load("Assets/speed.png"));

	m_needle = std::make_shared<cage::ui::UIElement>();
	m_needle->LoadTexture(IMG_Load("Assets/needle.png"));

	//m_button->MoveTo({ 600, 300 });

	m_fpsCounter = std::make_shared<cage::ui::UIElement>();
	m_root.Add(m_fpsCounter);
	m_fpsCounter->SetMounting(cage::ui::MountPoint::TOP_LEFT);
	m_fpsCounter->MoveTo({ 20, 0 });

	spriteVerShader.CompileFromFile("Assets/sprite.ver");
	spriteFragShader.CompileFromFile("Assets/sprite.frag");

	spriteProgram = std::make_shared<cage::SpriteShader>(spriteVerShader, spriteFragShader);
	spriteProgram->Use();

	//spriteProgram->Projection->value = glm::identity<glm::mat4>();
	spriteProgram->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
	spriteProgram->Projection->ForwardToShader();

	m_root.Resize({ size.first, size.second });
	m_speedometer->SetMounting(cage::ui::MountPoint::TOP_LEFT);
	m_root.Add(m_speedometer);
	m_needle->SetMounting(cage::ui::MountPoint::CENTER);
	m_speedometer->Add(m_needle);
	m_speedometer->MoveTo({ 880.f, 0.f });
	m_needle->MoveTo({ 0.f, 5.f });
	cage::ui::UIElement::shader = spriteProgram;

	camera->SetPosition({ 200, 30, 10 });

	m_needle->SetPivot({ 0.f, 12.f });

	m_menuOverlay = std::make_shared<cage::ui::UIElement>(false);

	using Ref = std::shared_ptr<cage::ui::UIElement>;
	Ref button = std::make_shared<cage::ui::Button>(std::make_shared<cage::Texture>(IMG_Load("Assets/button/idle.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/button/hover.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/button/pressed.png")));
	m_root.Add(button);
	button->SetMounting(cage::ui::MountPoint::TOP_LEFT);
	//Ref checkgroup = std::make_shared<cage::ui::CheckBoxGroup>();
	std::static_pointer_cast<cage::ui::Button>(button)->OnClick = [&]() { cage::Texture::MissingTexture = new cage::Texture(IMG_Load("Assets/simon.png")); };

	//s1.MoveTo(glm::vec3{ 120.0f, 19.f, 150.0f }).Play(Mix_LoadWAV("Assets/shrek.ogg"));
	//s2.MoveTo({ 200.f, 12.f, 200.f }).Play(Mix_LoadWAV("Assets/shrek2.ogg"));
	//s3.MoveTo({ 50.0f, 15.f, 50.f }).Play(Mix_LoadWAV("Assets/shrek1.ogg"));
	//s4.MoveTo({ 80.0f, 15.f, 120.0f }).Play(Mix_LoadWAV("Assets/vader.ogg"));
}

void Sandbox::ProcessEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			m_quit = true;
		}

		m_input.Raise(e);

		switch (e.type)
		{
		case SDL_QUIT:
			m_quit = true;
			break;
		case SDL_KEYDOWN:
			if (!e.key.repeat)
			{
				if (e.key.keysym.sym == SDLK_f)
				{
					SDL_SetRelativeMouseMode((SDL_bool)!SDL_GetRelativeMouseMode());
				}
				else if (e.key.keysym.sym == SDLK_u)
					camera->ToggleMode();
			}
			break;
		case SDL_MOUSEMOTION:
			{
				camera->yaw += (float)e.motion.xrel / 10;
				camera->pitch -= (float)e.motion.yrel / 10;
				car.heading += glm::vec2{ e.motion.xrel, e.motion.yrel };
			}
			break;
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				std::cout << "Resize to " << e.window.data1 << "x" << e.window.data2 << std::endl;
				glViewport(0, 0, e.window.data1, e.window.data2);

				program->Use();
				spriteProgram->Use();
				program->Use();
				program->Use();

				camera->SetProjection(glm::perspective(glm::quarter_pi<float>(), (float)e.window.data1 / (float)e.window.data2, 0.1f, 5000.f));

				spriteProgram->Use();

				spriteProgram->Projection->value = glm::ortho(0.f, (float)e.window.data1, (float)e.window.data2, 0.f);
				spriteProgram->Projection->ForwardToShader();
			}
			break;
		}

	}
}

static float i = 0.f;
const float g = -9.8;
float yVel = 0.f;
static float textTime = 0.f;
static bool tOn = false;

void Sandbox::Update(float delta)
{
	const Uint8* keys = SDL_GetKeyboardState(nullptr);
	const float SPEED = 50.f;
	if (keys[SDL_SCANCODE_A])
	{
		camera->MoveLeftRight(-SPEED * delta);
	}
	if (keys[SDL_SCANCODE_D])
	{
		camera->MoveLeftRight(SPEED * delta);
	}
	if (keys[SDL_SCANCODE_W])
	{
		camera->MoveForward(SPEED * delta);
	}
	if (keys[SDL_SCANCODE_S])
	{
		camera->MoveForward(-SPEED * delta);
	}
	if (keys[SDL_SCANCODE_Q])
	{
		camera->Move(delta * glm::vec3{ 0.0f,SPEED,0.f });
		yVel = 0;
	}
	if (keys[SDL_SCANCODE_E])
	{
		camera->Move(delta * glm::vec3{ 0.f,-SPEED,0.f });
	}
	if (keys[SDL_SCANCODE_SPACE])
	{
		i = 0;
	}

	//yVel += g * delta;
	textTime += delta;

	listener.MoveTo(camera->GetPosition()).LookAt(glm::normalize(camera->GetFront()), camera->GetRight());
	s1.UpdateListener(listener);
	s2.UpdateListener(listener);
	s3.UpdateListener(listener);
	s4.UpdateListener(listener);
	if (textTime > 0.1)
	{
		m_fpsCounter->LoadTexture(TTF_RenderText_Blended(m_font, std::string("FPS: " + std::to_string(1.0 / delta)).c_str(), FONT_COLOR));
		textTime = 0.f;
	}
	m_needle->SetRotation((std::abs(yVel / 128.0 * M_PI)) - 2 * M_PI / 3);

	camera->Move({ 0.f, yVel * delta, 0.f });

	auto v = camera->GetPosition() + glm::vec3{ 0.0f, -0.7f, 3.0f };

	int xIndex = std::floorf(v.x / 5.f), zIndex = std::floorf(v.z / 5.f);
	float dx = v.x / 5.f - xIndex;
	float dz = v.z / 5.f - zIndex;
	float yInterp1 = (1.0f - dx) * world.GetBuffer()[xIndex + (100 * zIndex)].position.y + dx * world.GetBuffer()[xIndex + 1 + (100 * zIndex)].position.y;
	float yInterp2 = (1.0f - dx) * world.GetBuffer()[xIndex + (100 * (zIndex + 1))].position.y + dx * world.GetBuffer()[xIndex + 1 + (100 * (zIndex + 1))].position.y;
	float yInterp = dz * yInterp2 + (1.0f - dz) * yInterp1;
	auto diff = v.y - yInterp;

	if (diff < 0)
	{
		camera->Move({ 0.0f, -diff, 0.0f });
		yVel = 0;
	}
	
	if (glm::distance(camera->GetPosition(), glm::vec3{ 200.f, 12.f, 200.f }) < 30.f && !tOn)
	{
		tOn = true;
		//Mix_PlayChannel(1, music, 0);
	}

}

void Sandbox::Draw()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program->Use();
	camera->Apply();
	program->Model->value = glm::translate(glm::identity<glm::mat4>(), glm::vec3{ 120.0f, 19.f, 150.0f });
	program->Model->ForwardToShader();
	shrek.Draw();
	//program->Model->value = glm::translate(glm::identity<glm::mat4>(), glm::vec3{ 80.0f, 15.f, 120.0f });
	//program->Model->ForwardToShader();
	//vader.Draw();
	program->Model->value = glm::translate(glm::identity<glm::mat4>(), { 50.0f, 15.f, 50.f });
	program->Model->ForwardToShader();
	car.Draw();
	program->Model->value = glm::rotate(glm::scale(glm::translate(glm::identity<glm::mat4>(), { 200.f, 12.f, 200.f }), { 3.f, 3.f, 3.f }), glm::pi<float>(), { 0.f, 1.f, 0.f });
	program->Model->ForwardToShader();
	thanos.Draw();
	program->Model->value = glm::identity<glm::mat4>();
	program->Model->ForwardToShader();
	
	world.DrawIndexed(100 * 100 * 6);

	glDisable(GL_DEPTH_TEST);
	spriteProgram->Use();

	m_root.Draw();
}

std::vector<cage::Vertex3UVNormal> Sandbox::genTerrain()
{
	size_t size = 100;
	std::vector<cage::Vertex3UVNormal> vertices;
	std::vector<glm::vec3> positions, normals;
	std::vector<unsigned int> indices;
	vertices.reserve(size * size);
	normals.resize(size * size);
	positions.reserve(size * size);
	float height = 0.f;
	const float scale = 5.f;
	const float heightScale = 2.f;
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			height = 0;
			float temp = Perlin::OctavePerlin3((float)x / 40.f, (float)y / 40.f, 4.f, 3, 0.5) * heightScale * 12;
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
	for (size_t y = 1; y < size - 1; y++)
	{
		for (size_t x = 1; x < size - 1; x++)
		{
			float dx = positions[y * size + x + 1].y - positions[y * size + x - 1].y;
			float dz = positions[(y + 1) * size + x].y - positions[(y - 1) * size + x].y;

			normals[y * size + x] = glm::normalize(glm::vec3{ -dx * 0.5f, 1.f, -dz * 0.5f });
		}
	}
	// Construct final geometry
	for (int i = 0; i < size * size; i++)
	{
		vertices.emplace_back(positions[i], glm::vec2{ 1.0 - (float)(i % size) / size, (float)(i / size) / size * 2 }, normals[i]);
	}

	glCreateBuffers(1, &m_eboid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	return vertices;
}