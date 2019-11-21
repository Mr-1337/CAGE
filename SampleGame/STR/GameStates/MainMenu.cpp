#include "MainMenu.hpp"
#include "Sandbox.hpp"
#include "Lobby.hpp"
#include "../../../CAGE/Core/StateMachine.hpp"

MainMenu::MainMenu(std::pair<int, int> size)
{
	glClearColor(0.3f, 0.3f, 0.2f, 1.f);

	cage::Texture::MissingTexture = new cage::Texture(IMG_Load("Assets/missing.png"));

	cage::Shader vertex(cage::Shader::VERTEX), fragment(cage::Shader::FRAGMENT);
	vertex.CompileFromFile("Assets/sprite.ver");
	fragment.CompileFromFile("Assets/sprite.frag");
	m_spriteShader = std::make_shared<cage::SpriteShader>(vertex, fragment);

	m_spriteShader->Use();

	m_spriteShader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
	m_spriteShader->Projection->ForwardToShader();
	
	cage::ui::UIElement::shader = m_spriteShader;

	//MenuButton::s_font = TTF_OpenFont("Assets/sans.ttf", 36);

	using Ref = std::shared_ptr<cage::ui::UIElement>;

	Ref button1 = std::make_shared<MenuButton>("Play");
	Ref button2 = std::make_shared<MenuButton>("Multiplayer");
	Ref button3 = std::make_shared<MenuButton>("Quit");

	button1->SetMounting(cage::ui::MountPoint::TOP_LEFT);
	button2->SetMounting(cage::ui::MountPoint::TOP_LEFT);
	button3->SetMounting(cage::ui::MountPoint::TOP_LEFT);

	button1->MoveTo({ 200.f, 0.f });
	button2->MoveTo({ 200.f, 300.f });
	button3->MoveTo({ 200.f, 600.f });

	m_root.Add(button1);
	m_root.Add(button2);
	m_root.Add(button3);

	std::static_pointer_cast<MenuButton>(button1)->OnClick = [&, size]() { s_stateMachine->Push(new Sandbox(size)); };
	std::static_pointer_cast<MenuButton>(button2)->OnClick = [&, size]() { s_stateMachine->Push(new Lobby(size)); };
}

void MainMenu::ProcessEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			m_quit = true;
			break;
		case SDL_MOUSEMOTION:
			cage::MouseMotionEvent::RaiseEvent({ e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel });
			break;
		case SDL_MOUSEBUTTONDOWN:
			cage::MouseClickEvent::RaiseEvent({ e.button.x, e.button.y, (cage::MouseClickEvent::MouseButton)e.button.button, false });
			break;
		case SDL_MOUSEBUTTONUP:
			cage::MouseClickEvent::RaiseEvent({ e.button.x, e.button.y, (cage::MouseClickEvent::MouseButton)e.button.button, true });
			break;
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
		}
	}
}

void MainMenu::Update(float delta)
{

}

void MainMenu::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Draw UI
	glDisable(GL_DEPTH_TEST);
	m_root.Draw();
}