#include "ShrekMenu.hpp"
#include "Gameplay.hpp"
#include "Lobby.hpp"
#include "../CAGE/Core/StateMachine.hpp"
#include "../CAGE/Core/Game.hpp"

ShrekMenu::ShrekMenu(cage::Game& game) : cage::GameState(game)
{
	glClearColor(0.3f, 0.5f, 0.2f, 0.0f);

	cage::Texture::MissingTexture = new cage::Texture(IMG_Load("Assets/missing.png"));

	cage::Shader vertex(cage::Shader::ShaderType::VERTEX), fragment(cage::Shader::ShaderType::FRAGMENT);
	vertex.CompileFromFile("Assets/sprite.vert");
	fragment.CompileFromFile("Assets/sprite.frag");
	m_spriteShader = std::make_shared<cage::SpriteShader>(vertex, fragment);

	auto size = getGame().GetWindow().GetSize();

	m_spriteShader->Use();

	m_spriteShader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
	m_spriteShader->Projection->ForwardToShader();
	
	cage::ui::UIElement::shader = m_spriteShader;

	//MenuButton::s_font = TTF_OpenFont("Assets/sans.ttf", 36);
	MenuButton::s_font = new cage::Font("Assets/sans.ttf", 36);

	Ref button1 = std::make_shared<MenuButton>("Play");
	Ref button2 = std::make_shared<MenuButton>("Multiplayer");
	Ref button3 = std::make_shared<MenuButton>("Options");
	Ref button4 = std::make_shared<MenuButton>("Quit");

	m_buttonGroup = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::GridLayout({ 0.f, 50.f }, 1));

	std::static_pointer_cast<MenuButton>(button1)->OnClick = [&, size]() { s_stateMachine->Push(new Gameplay(getGame(), 10, 20)); };
	std::static_pointer_cast<MenuButton>(button2)->OnClick = [&, size]() { s_stateMachine->Push(new Lobby(getGame())); };
	std::static_pointer_cast<MenuButton>(button3)->OnClick = [&, size]() { };
	std::static_pointer_cast<MenuButton>(button4)->OnClick = [&, size]() { quit(); };

	m_buttonGroup->Add(button1);
	m_buttonGroup->Add(button2);
	m_buttonGroup->Add(button3);
	m_buttonGroup->Add(button4);

	m_input.Subscribe(button1.get());
	m_input.Subscribe(button2.get());
	m_input.Subscribe(button3.get());
	m_input.Subscribe(button4.get());

	m_root.SetLocalMounting(cage::ui::MountPoint::CENTER);
	m_root.Resize({ (float)size.first, (float)size.second });
	m_root.SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);
	m_root.Add(m_buttonGroup);

}

void ShrekMenu::ProcessEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		m_input.Raise(e);
		switch (e.type)
		{
		case SDL_WINDOWEVENT:
			switch (e.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				glViewport(0, 0, e.window.data1, e.window.data2);
				m_root.Resize({ (float)e.window.data1, (float)e.window.data2 });
				m_spriteShader->Projection->value = glm::ortho(0.f, (float)e.window.data1, (float)e.window.data2, 0.f);
				m_spriteShader->Projection->ForwardToShader();
				break;
			}
			break;
		case SDL_QUIT:
			quit();
			break;
		}
	}
}

void ShrekMenu::OnRevealed()
{
	auto size = getGame().GetWindow().GetSize();
	glViewport(0, 0, size.first, size.second);
	m_root.Resize({ (float)size.first, (float)size.second });
	m_spriteShader->Projection->value = glm::ortho(0.f, (float)size.first, (float)size.second, 0.f);
	m_spriteShader->Projection->ForwardToShader();
}

void ShrekMenu::OnHidden()
{

}

void ShrekMenu::Update(float delta)
{
	//m_root.Rotate(delta * 0.5);
}

void ShrekMenu::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Draw UI
	glDisable(GL_DEPTH_TEST);
	m_spriteShader->Use();
	m_root.Draw();
}