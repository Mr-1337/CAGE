#include <Glad/glad/glad.h>
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "UITest.hpp"
#include "../MenuButton.hpp"

UITest::UITest(std::pair<int, int> size)
{
	{
		cage::Shader ver(cage::Shader::VERTEX), frag(cage::Shader::FRAGMENT);
		ver.CompileFromFile("Assets/sprite.vert");
		frag.CompileFromFile("Assets/sprite.frag");

		m_spriteShader = std::make_unique<cage::SpriteShader>(ver, frag);
	}

	m_spriteShader->Use();
	m_spriteShader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
	m_spriteShader->Projection->ForwardToShader();

	glClearColor(0.5, 0.0, 0.0, 1.0);

	cage::ui::UIElement::shader = m_spriteShader;

	//b1 = std::make_shared<cage::ui::Button>(std::make_shared<cage::Texture>(IMG_Load("Assets/simon.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/simon2.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/ms.png")));
	//b2 = std::make_shared<cage::ui::Button>(std::make_shared<cage::Texture>(IMG_Load("Assets/simon.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/simon2.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/ms.png")));
	//b3 = std::make_shared<cage::ui::Button>(std::make_shared<cage::Texture>(IMG_Load("Assets/simon.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/simon2.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/ms.png")));
	//b4 = std::make_shared<cage::ui::Button>(std::make_shared<cage::Texture>(IMG_Load("Assets/simon.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/simon2.png")), std::make_shared<cage::Texture>(IMG_Load("Assets/ms.png")));

	b1 = std::make_shared<MenuButton>("1");
	b2 = std::make_shared <MenuButton>("2");
	b3 = std::make_shared <MenuButton>("3");
	b4 = std::make_shared <MenuButton>("4");

	auto font = TTF_OpenFont("Assets/sans.ttf", 36);

	c1 = std::make_shared<cage::ui::CheckBox>("Yee", font);
	c2 = std::make_shared<cage::ui::CheckBox>("Haw", font);
	c3 = std::make_shared<cage::ui::CheckBox>("Frog", font);

	message = std::make_shared<cage::ui::Text>(font);
	message->SetColor({ 0, 0, 255, 255 });
	message->SetText("Still waiting...");

	//b4->MoveTo({ 300, 300 });

	m_root.Add(b1);
	b1->Add(message);
	b1->Add(b2);
	b2->Add(b3);
	b3->Add(b4);

	m_root.Add(c1);
	m_root.Add(c2);
	m_root.Add(c3);

	b1->SetParentMounting(cage::ui::MountPoint::CENTER);
	b2->SetLocalMounting(cage::ui::MountPoint::TOP_RIGHT);
	b2->SetParentMounting(cage::ui::MountPoint::BOTTOM_RIGHT);
	b3->SetLocalMounting(cage::ui::MountPoint::CENTER_LEFT);
	b3->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);
	b4->SetLocalMounting(cage::ui::MountPoint::CENTER_RIGHT);
	b4->SetParentMounting(cage::ui::MountPoint::BOTTOM_RIGHT);

	b2->Scale(0.8);
	b3->Scale(0.5);
	b4->Scale(0.5);

	c1->MoveTo({ 800, 700 });
	c2->MoveTo({ 0, 100 });
	c3->MoveTo({ 800, 600 });

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

	m_root.Resize({ size.first, size.second });
	m_root.MoveTo({ size.first / 2, size.second / 2 });

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

				m_root.Resize({ e.window.data1, e.window.data2 });
				m_root.MoveTo({ e.window.data1 / 2, e.window.data2 / 2 });

				break;
			}
			break;
		case SDL_MOUSEWHEEL:
			if (e.wheel.y > 0)
				b1->Scale(1.01f);
			else
			{
				b1->Scale(0.99f);
				message->SetText("REEEEEE");
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

	int fast = c1->Checked();

	b1->Rotate(dt * (0.11 + fast));
	b2->Rotate(dt * (0.23 + fast));
	b3->Rotate(dt * (0.34 + fast));

	b4->Rotate(dt * (0.64 + fast));

	glm::vec2 total = { 0.f, 0.f };

	if (c3->Checked())
	{
		total += glm::vec2{ 200.f * sin(6.28 * tot), 200.f * cos(6.28 * tot) };
	}
	if (c2->Checked())
	{
		total += glm::vec2{ 200.f * sin(6.28 * 0.03333 * tot), 0.f };
	}

	message->MoveTo(total);

}

void UITest::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	m_root.Draw();
}
