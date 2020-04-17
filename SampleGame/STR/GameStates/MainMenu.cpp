#include "MainMenu.hpp"
#include "Sandbox.hpp"
#include "Lobby.hpp"
#include "UITest.hpp"
#include "Editor.hpp"
#include "../../../CAGE/Core/StateMachine.hpp"

MainMenu::MainMenu(std::pair<int, int> size)
{
	glClearColor(0.3f, 0.3f, 0.2f, 1.f);

	cage::Texture::MissingTexture = new cage::Texture(IMG_Load("Assets/missing.png"));

	cage::Shader vertex(cage::Shader::VERTEX), fragment(cage::Shader::FRAGMENT);
	vertex.CompileFromFile("Assets/sprite.vert");
	fragment.CompileFromFile("Assets/sprite.frag");
	m_spriteShader = std::make_shared<cage::SpriteShader>(vertex, fragment);

	m_spriteShader->Use();

	m_spriteShader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
	m_spriteShader->Projection->ForwardToShader();
	
	cage::ui::UIElement::shader = m_spriteShader;

	//MenuButton::s_font = TTF_OpenFont("Assets/sans.ttf", 36);

	Ref button1 = std::make_shared<MenuButton>("Play");
	Ref button2 = std::make_shared<MenuButton>("Multiplayer");
	Ref button3 = std::make_shared<MenuButton>("Transform Testing");
	Ref button4 = std::make_shared<MenuButton>("Editor");
	Ref button5 = std::make_shared<MenuButton>("Quit");

	button1->SetLocalMounting(cage::ui::MountPoint::TOP);
	button2->SetLocalMounting(cage::ui::MountPoint::TOP);
	button3->SetLocalMounting(cage::ui::MountPoint::TOP);
	button4->SetLocalMounting(cage::ui::MountPoint::TOP);
	button5->SetLocalMounting(cage::ui::MountPoint::TOP);

	button1->SetParentMounting(cage::ui::MountPoint::TOP);
	button2->SetParentMounting(cage::ui::MountPoint::TOP);
	button3->SetParentMounting(cage::ui::MountPoint::TOP);
	button4->SetParentMounting(cage::ui::MountPoint::TOP);
	button5->SetParentMounting(cage::ui::MountPoint::TOP);

	button1->MoveTo({ 0.f, 0.f });
	button2->MoveTo({ 0.f, 150.f });
	button3->MoveTo({ 0.f, 300.f });
	button4->MoveTo({ 0.f, 450.f });
	button5->MoveTo({ 0.f, 600.f });

	std::cout << size.first << ", " << size.second << std::endl;

	Add(button1);
	Add(button2);
	Add(button3);
	Add(button4);
	Add(button5);

	std::static_pointer_cast<MenuButton>(button1)->OnClick = [&, size]() { s_stateMachine->Push(new Sandbox(size)); };
	std::static_pointer_cast<MenuButton>(button2)->OnClick = [&, size]() { s_stateMachine->Push(new Lobby(size)); };
	std::static_pointer_cast<MenuButton>(button3)->OnClick = [&, size]() { s_stateMachine->Push(new UITest(size)); };
	std::static_pointer_cast<MenuButton>(button4)->OnClick = [&, size]() { s_stateMachine->Push(new Editor(size)); };
	std::static_pointer_cast<MenuButton>(button5)->OnClick = [&, size]() { m_quit = true; };

	m_root.SetLocalMounting(cage::ui::MountPoint::CENTER);
	m_root.Resize({ (float)size.first, (float)size.second });
	m_root.MoveTo({ (float)size.first / 2, (float)size.second / 2 });

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
		case SDL_WINDOWEVENT:
			switch (e.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				auto size = std::make_pair(e.window.data1, e.window.data2);
				glViewport(0, 0, e.window.data1, e.window.data2);
				m_root.Resize({ (float)e.window.data1, (float)e.window.data2 });
				m_root.MoveTo({ (float)e.window.data1/2, (float)e.window.data2/2 });
				m_spriteShader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
				m_spriteShader->Projection->ForwardToShader();
				break;
			}
			break;
		default:
			m_input.Raise(e);
		}
	}
}

void MainMenu::Add(Ref thingy)
{
	m_root.Add(thingy);

	cage::EventListener* l = dynamic_cast<cage::EventListener*>(thingy.get());
	m_input.Subscribe(l);
}

void MainMenu::Update(float delta)
{
	//m_root.Rotate(delta * 0.5);
}

void MainMenu::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Draw UI
	glDisable(GL_DEPTH_TEST);
	m_root.Draw();
}