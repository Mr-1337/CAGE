#include "Sandbox.hpp"

Sandbox::Sandbox(cage::Game& game, std::pair<int, int> size) : 
	cage::GameState(game),
	verShader(cage::Shader::ShaderType::VERTEX),
	fragShader(cage::Shader::ShaderType::FRAGMENT),
	spriteVerShader(cage::Shader::ShaderType::VERTEX),
	spriteFragShader(cage::Shader::ShaderType::FRAGMENT),
	world("World", true)
{

	totalTime = 0;
	iplCreateContext(nullptr, nullptr, nullptr, &context);


	IPLRenderingSettings settings{ samplingrate, framesize };

	IPLHrtfParams hrtfParams{ IPL_HRTFDATABASETYPE_DEFAULT, nullptr, 0};
	iplCreateBinauralRenderer(context, settings, hrtfParams, &m_audioRenderer);

	IPLAudioFormat input, output;
	input.channelLayoutType	= IPL_CHANNELLAYOUTTYPE_SPEAKERS;
	input.channelLayout		= IPL_CHANNELLAYOUT_SEVENPOINTONE;
	input.channelOrder		= IPL_CHANNELORDER_INTERLEAVED;

	output.channelLayoutType = IPL_CHANNELLAYOUTTYPE_SPEAKERS;
	output.channelLayout	 = IPL_CHANNELLAYOUT_SEVENPOINTONE;
	output.channelOrder		 = IPL_CHANNELORDER_INTERLEAVED;

	effect = nullptr;
	iplCreateBinauralEffect(m_audioRenderer, input, output, &effect);
	
	iplCreateDirectSoundEffect(output, output, settings, &effect2);

	Mix_Chunk* sound = Mix_LoadWAV("Assets/shrek.ogg");

	inBuffer = { input, framesize, (IPLfloat32*)sound->abuf };

	srcBuff = (float*)sound->abuf;
	audioBuffer.resize(framesize * 8);

	iplCreateEnvironment(context, nullptr, {}, nullptr, nullptr, &m_envRenderer);

	// get and print the audio format in use
	int numtimesopened, frequency, channels;
	Uint16 format;
	numtimesopened = Mix_QuerySpec(&frequency, &format, &channels);
	if (!numtimesopened) {
		printf("Mix_QuerySpec: %s\n", Mix_GetError());
	}
	else {
		const char* format_str = "Unknown";
		switch (format) {
		case AUDIO_U8: format_str = "U8"; break;
		case AUDIO_S8: format_str = "S8"; break;
		case AUDIO_U16LSB: format_str = "U16LSB"; break;
		case AUDIO_S16LSB: format_str = "S16LSB"; break;
		case AUDIO_U16MSB: format_str = "U16MSB"; break;
		case AUDIO_S16MSB: format_str = "S16MSB"; break;
		case AUDIO_F32LSB: format_str = "F32LSB"; break;
		}
		printf("opened=%d times  frequency=%dHz  format=%s  channels=%d",
			numtimesopened, frequency, format_str, channels);
	}

	outBuffer = { output, framesize, audioBuffer.data() };

	shrekSong.allocated = 0;
	shrekSong.abuf = (Uint8*)audioBuffer.data();
	shrekSong.alen = audioBuffer.size() * sizeof(float);
	shrekSong.volume = 127;


	//Mix_PlayChannel(3, sound, -1);

	world.BindVAO();
	world.SetGeometry(genTerrain());
	//glClearColor(0.4f, 0.5f, 0.2f, 0.0f);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	shrek.LoadModel("Assets/shrek.obj");
	//thanos.LoadModel("Assets/thanos.obj");
	//vader.LoadModel("Assets/vader.obj");

	//SDL_assert(controller != nullptr);

	verShader.CompileFromFile("Assets/generic3D.vert");
	fragShader.CompileFromFile("Assets/generic3D.frag");

	SDL_AudioSpec spec;
	spec.freq = 44100;
	spec.channels = 8;
	spec.format = AUDIO_F32LSB;
	spec.samples = 1024;
	spec.callback = nullptr;

	m_font = new cage::Font("Assets/sans.ttf", 36);
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

	m_fpsCounter = std::make_shared<cage::ui::Text>(*m_font);
	m_root.Add(m_fpsCounter);
	m_fpsCounter->SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);
	m_fpsCounter->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);
	//m_fpsCounter->MoveTo({ 20, 20 });

	spriteVerShader.CompileFromFile("Assets/sprite.vert");
	spriteFragShader.CompileFromFile("Assets/sprite.frag");

	spriteProgram = std::make_shared<cage::SpriteShader>(spriteVerShader, spriteFragShader);
	spriteProgram->Use();

	spriteProgram->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
	spriteProgram->Projection->ForwardToShader();

	m_root.Resize({ size.first, size.second });
	m_speedometer->SetLocalMounting(cage::ui::MountPoint::TOP_RIGHT);
	m_speedometer->SetParentMounting(cage::ui::MountPoint::TOP_RIGHT);
	m_root.Add(m_speedometer);
	m_needle->SetLocalMounting(cage::ui::MountPoint::BOTTOM);
	m_needle->MoveTo({ 0.f, 5.f });
	m_speedometer->Add(m_needle);
	cage::ui::UIElement::shader = spriteProgram;

	camera->SetPosition({ 200, 30, 10 });

	m_menuOverlay = std::make_shared<cage::ui::UIElement>(false);

	//s1.MoveTo(glm::vec3{ 120.0f, 19.f, 150.0f }).Play(Mix_LoadWAV("Assets/shrek.ogg"));
	//s2.MoveTo({ 200.f, 12.f, 200.f }).Play(Mix_LoadWAV("Assets/shrek2.ogg"));
	//s3.MoveTo({ 50.0f, 15.f, 50.f }).Play(Mix_LoadWAV("Assets/shrek1.ogg"));
	//s4.MoveTo({ 80.0f, 15.f, 120.0f }).Play(Mix_LoadWAV("Assets/vader.ogg"));

	m_root.Resize(glm::vec2{ size.first, size.second });
	m_root.MoveTo(0.5f * glm::vec2{ size.first, size.second });
}

void Sandbox::ProcessEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			quit();
		}

		m_input.Raise(e);

		switch (e.type)
		{
		case SDL_QUIT:
			quit();
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

				m_root.Resize(glm::vec2{ e.window.data1, e.window.data2 });
				m_root.MoveTo(0.5f * glm::vec2{ e.window.data1, e.window.data2 });
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

	yVel += g * delta;
	textTime += delta;

	totalTime += delta;

	//std::cout << totalTime << std::endl;

	static int soundFrame = 0;

	if (totalTime >=  (1024.f / 44100.f))
	{

		soundFrame += 1;

		IPLVector3 offset, pos;
		auto v = glm::normalize(glm::vec3{ 120.0f, 19.f, 150.0f } - camera->GetPosition());

		auto v2 = camera->GetPosition();
		pos = { v2.x, v2.y, v2.z };

		auto front = camera->GetFront();
		auto right = camera->GetRight();
		auto up = glm::cross(right, front);

		offset.x = v.x;
		offset.y = v.y;
		offset.z = v.z;

		IPLSource source;
		IPLDirectivity d;

		d.dipolePower = 0.8;
		d.dipoleWeight = 0.7;
		d.callback = nullptr;

		source.position = { 120.0f, 19.f, 150.0f };
		source.ahead = { 1.0, 0, 0 };
		source.up = { 0, 1, 0 };
		source.right = { 0, 0, 1 };
		source.distanceAttenuationModel = { IPL_DISTANCEATTENUATION_DEFAULT };
		source.airAbsorptionModel = { IPL_AIRABSORPTION_DEFAULT };
		source.directivity = d;

		IPLDirectSoundPath path = iplGetDirectSoundPath(m_envRenderer, pos, { front.x, front.y, front.z }, { up.x, up.y, up.z }, source, 10.f, 0, IPL_DIRECTOCCLUSION_NONE, IPL_DIRECTOCCLUSION_RAYCAST);

		IPLDirectSoundEffectOptions options;
		options.applyAirAbsorption = IPL_FALSE;
		options.applyDirectivity = IPL_FALSE;
		options.applyDistanceAttenuation = IPL_TRUE;
		options.directOcclusionMode = IPL_DIRECTOCCLUSION_NONE;

		//iplApplyDirectSoundEffect(effect2, inBuffer, path, options, outBuffer);
		iplApplyBinauralEffect(effect, m_audioRenderer, inBuffer, offset, IPL_HRTFINTERPOLATION_NEAREST, 1, outBuffer);

		inBuffer.interleavedBuffer += framesize * 4;

		//SDL_memcpy(audioBuffer.data(), &srcBuff[soundFrame * 1024 * 8], 1024 * 8 * sizeof(float));

		//std::cout << SDL_QueueAudio(2, audioBuffer.data(), audioBuffer.size() * sizeof(float));
		//std::cout << SDL_GetError() << '\n';

		Mix_PlayChannel(1, &shrekSong, 0);

		totalTime = 0;
	}

	//s1.UpdateListener(listener);
	//s2.UpdateListener(listener);
	//s3.UpdateListener(listener);
	//s4.UpdateListener(listener);
	if (textTime > 0.05)
	{
		m_fpsCounter->SetText(std::string("FPS: " + std::to_string(1.0 / delta)).c_str());
		textTime = 0.f;
	}
	m_needle->SetRotation((std::abs(yVel / 128.0 * M_PI)) - 2 * M_PI / 3);

	camera->Move({ 0.f, yVel * delta, 0.f });

	auto v = camera->GetPosition() + glm::vec3{ 0.0f, -0.7f, 3.0f };

	int xIndex = std::floor(v.x / 5.f), zIndex = std::floor(v.z / 5.f);
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
	//program->Model->value = glm::rotate(glm::scale(glm::translate(glm::identity<glm::mat4>(), { 200.f, 12.f, 200.f }), { 3.f, 3.f, 3.f }), glm::pi<float>(), { 0.f, 1.f, 0.f });
	//program->Model->ForwardToShader();
	//thanos.Draw();
	program->Model->value = glm::identity<glm::mat4>();
	program->Model->ForwardToShader();
	
	world.DrawIndexed(100 * 100 * 6);

	glDisable(GL_DEPTH_TEST);
	spriteProgram->Use();

	m_root.Draw();
}

void Sandbox::OnRevealed()
{

}

void Sandbox::OnHidden()
{

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