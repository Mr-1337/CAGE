#include <Glad/glad/glad.h>
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "UITest.hpp"

UITest::UITest(std::pair<int, int> size)
{
	{
		cage::Shader ver(cage::Shader::VERTEX), frag(cage::Shader::FRAGMENT);
		ver.CompileFromFile("Assets/sprite.ver");
		frag.CompileFromFile("Assets/sprite.frag");

		m_spriteShader = std::make_unique<cage::SpriteShader>(ver, frag);
	}

	m_spriteShader->Use();
	m_spriteShader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
	m_spriteShader->Projection->ForwardToShader();

	glClearColor(0.5, 0.0, 0.0, 1.0);

	cage::ui::UIElement::shader = m_spriteShader;

	b1 = std::make_shared<cage::ui::Button>(std::make_shared<cage::Texture>(IMG_Load("Assets/simon.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/simon2.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/ms.png")));
	b2 = std::make_shared<cage::ui::Button>(std::make_shared<cage::Texture>(IMG_Load("Assets/simon.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/simon2.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/ms.png")));
	b3 = std::make_shared<cage::ui::Button>(std::make_shared<cage::Texture>(IMG_Load("Assets/simon.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/simon2.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/ms.png")));
	b4 = std::make_shared<cage::ui::Button>(std::make_shared<cage::Texture>(IMG_Load("Assets/simon.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/simon2.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/ms.png")));

	auto font = TTF_OpenFont("Assets/sans.ttf", 36);

	c1 = std::make_shared<cage::ui::CheckBox>("Has the big", font);
	c2 = std::make_shared<cage::ui::CheckBox>("gay", font);
	c3 = std::make_shared<cage::ui::CheckBox>("frog", font);

	m_root.Resize({ size.first, size.second });

	b1->MoveTo({ 300, 300 });

	m_root.Add(b1);
	b1->Add(b2);
	b2->Add(b3);
	b3->Add(b4);
	b1->Add(c1);
	b1->Add(c2);
	
	m_root.Add(c3);

	b2->SetMounting(cage::ui::MountPoint::TOP_LEFT);
	b3->SetMounting(cage::ui::MountPoint::TOP_LEFT);
	b4->SetMounting(cage::ui::MountPoint::TOP_LEFT);

	b3->Scale(0.5);
	b4->Scale(0.5);
	b4->MoveTo({ 20.f, 20.f });

	m_input.Subscribe((cage::EventListener*)b1.get());
	m_input.Subscribe((cage::EventListener*)b2.get());
	m_input.Subscribe((cage::EventListener*)b3.get());
	m_input.Subscribe((cage::EventListener*)b4.get());
	m_input.Subscribe((cage::EventListener*)c1.get());
	m_input.Subscribe((cage::EventListener*)c2.get());
	m_input.Subscribe((cage::EventListener*)c3.get());

	s1 = Mix_LoadWAV("Assets/layers1.ogg");
	s2 = Mix_LoadWAV("Assets/layers2.ogg");
	s3 = Mix_LoadWAV("Assets/layers3.ogg");
	s4 = Mix_LoadWAV("Assets/biglayers.ogg");

	b1->OnClick = [this]() { Mix_PlayChannel(1, s1, 0); };
	b2->OnClick = [this]() { Mix_PlayChannel(1, s2, 0); };
	b3->OnClick = [this]() { Mix_PlayChannel(1, s3, 0); };
	b4->OnClick = [this]() { Mix_PlayChannel(1, s4, 0); };

}

void UITest::ProcessEvents()
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
				auto newSize = std::make_pair(e.window.data1, e.window.data2);
				glViewport(0, 0, e.window.data1, e.window.data2);
				m_spriteShader->Projection->value = glm::ortho(0.f, (float)newSize.first, (float)newSize.second, 0.f);
				m_spriteShader->Projection->ForwardToShader();
				break;
			}
			break;
		default:
			m_input.Raise(e);
		}
	}
}

static float tot = 0;

void UITest::Update(float dt)
{
	tot += dt;
	b1->MoveTo({ 300.f, 300.f });
	b2->SetScale(0.5f + .25f * sin(tot));
	b2->MoveTo({ 30.f * cos(tot), 0.f });
	b2->Rotate(dt * 0.5);
	b3->Rotate(dt);
	b3->MoveTo({ fmodf(tot, 100.f), 0.f });
}

void UITest::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	m_root.Draw();
}
