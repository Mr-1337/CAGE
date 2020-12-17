#include "Lobby.hpp"
#include "../../../CAGE/Core/StateMachine.hpp"

Lobby::Lobby(cage::Game& game, std::pair<int, int> size) : cage::GameState(game), WAIT_TIME(500), m_sound(nullptr)
{
	glClearColor(0.3f, 0.3f, 0.2f, 1.f);

	cage::Shader vertex(cage::Shader::ShaderType::VERTEX), fragment(cage::Shader::ShaderType::FRAGMENT), vertexSprite(cage::Shader::ShaderType::VERTEX), fragmentSprite(cage::Shader::ShaderType::FRAGMENT);
	vertexSprite.CompileFromFile("Assets/sprite.ver");
	fragmentSprite.CompileFromFile("Assets/sprite.frag");

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

	vertex.CompileFromSrcString(vsString);
	fragment.CompileFromSrcString(fsString);
	m_shader = new cage::Generic3DShader(vertex, fragment);
	m_camera = new cage::Camera(*m_shader);
	m_shader->Use();
	m_camera->SetProjection(glm::perspective(glm::quarter_pi<float>(), (float)size.first / size.second, 0.0001f, 5000.f));

	m_spriteShader = std::make_shared<cage::SpriteShader>(vertex, fragment);

	m_spriteShader->Use();

	m_spriteShader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
	m_spriteShader->Projection->ForwardToShader();

	cage::ui::UIElement::shader = m_spriteShader;

	using Ref = std::shared_ptr<cage::ui::UIElement>;

	shrek.LoadModel("Assets/shrek.obj");
	vader.LoadModel("Assets/vader.obj");
	thanos.LoadModel("Assets/Thanos.obj");
	cube.LoadModel("Assets/cube.obj");
	car.LoadModel("Assets/car.obj");

	m_sound = Mix_LoadWAV("Assets/mush.ogg");

	m_camera->SetPosition({ 3.0, 0.6, 0.0 });
	SDL_SetRelativeMouseMode(SDL_TRUE);

	m_rotating = false;
	m_selection = 0;
	m_theta = 0;

}

void Lobby::OnRevealed()
{

}

void Lobby::OnHidden()
{

}

void Lobby::ProcessEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			quit();
			break;
		/*
		case SDL_MOUSEMOTION:
			cage::MouseMotionEvent::RaiseEvent({ e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel });
			m_camera->yaw += (float)e.motion.xrel / 10;
			m_camera->pitch -= (float)e.motion.yrel / 10;
			break;
		case SDL_MOUSEBUTTONDOWN:
			cage::MouseClickEvent::RaiseEvent({ e.button.x, e.button.y, (cage::MouseClickEvent::MouseButton)e.button.button, false });
			break;
		case SDL_MOUSEBUTTONUP:
			cage::MouseClickEvent::RaiseEvent({ e.button.x, e.button.y, (cage::MouseClickEvent::MouseButton)e.button.button, true });
			break;
			*/
		case SDL_WINDOWEVENT:
			switch (e.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				auto size = std::make_pair(e.window.data1, e.window.data2);
				glViewport(0, 0, e.window.data1, e.window.data2);
				m_spriteShader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
				m_spriteShader->Projection->ForwardToShader();
				break;
			}
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_u)
				m_camera->ToggleMode();
			else if (e.key.keysym.sym == SDLK_a && !e.key.repeat)
			{
				if (!m_rotating)
				{
					m_selection++;
					m_selection %= 5;
					m_timer = std::chrono::high_resolution_clock::now();
					m_rotating = true;
					m_rotationVel = 0.2 * glm::two_pi<float>() / 0.5;
				}
			}
			else if (e.key.keysym.sym == SDLK_d && !e.key.repeat)
			{
				if (!m_rotating)
				{
					m_selection--;
					m_selection %= 5;
					m_timer = std::chrono::high_resolution_clock::now();
					m_rotating = true;
					m_rotationVel = -0.2 * glm::two_pi<float>() / 0.5;
				}
			}
			break;
		}
	}
}

void Lobby::Update(float delta)
{
	if (m_rotating && (std::chrono::high_resolution_clock::now() - m_timer) >= WAIT_TIME)
	{
		m_rotating = false;
		m_rotationVel = 0;
		std::cout << "MUSH\n";
		Mix_PlayChannel(1, m_sound, 0);
		m_timer = std::chrono::high_resolution_clock::now();
	}
	if (m_rotating)
	{
		m_theta += m_rotationVel * delta;
	}
}



void Lobby::Draw()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_shader->Use();
	m_camera->Apply();
	m_shader->Model->value = glm::identity<glm::mat4>();
	m_shader->Model->ForwardToShader();
	float charRot = m_theta;
	auto thing = [&](cage::Model & m)
	{
		charRot += glm::two_pi<float>() * 0.2;
		m_shader->Model->value = glm::rotate(glm::translate(glm::rotate(glm::identity<glm::mat4>(), charRot, { 0.f, 1.f, 0.f }), { 7.f, 0.f, 0.f }), -charRot - glm::half_pi<float>(), { 0.f, 1.f, 0.f });
		m_shader->Model->ForwardToShader();
		m.Draw();
	};
	
	thing(shrek);
	thing(vader);
	thing(thanos);
	thing(cube);
	thing(car);

	m_shader->Model->value = glm::translate(glm::identity<glm::mat4>(), { 7.0, -1.f, 0.f });
	m_shader->Model->ForwardToShader();

	cube.Draw();

	// Draw UI
	glDisable(GL_DEPTH_TEST);
	m_root.Draw();
}