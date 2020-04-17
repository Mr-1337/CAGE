#include "Editor.hpp"

Editor::Editor(std::pair<int, int> size)
{
	glClearColor(0.1f, 0.3f, 0.1f, 1.0f);

	{
		cage::Shader genVer(cage::Shader::VERTEX);
		cage::Shader genFrag(cage::Shader::FRAGMENT);

		genVer.CompileFromFile("Assets/generic3D.vert");
		genVer.CompileFromFile("Assets/generic3D.frag");

		m_genericShader = std::make_shared<cage::Generic3DShader>(genVer, genFrag);

		cage::Shader spriteVer(cage::Shader::VERTEX);
		cage::Shader spriteFrag(cage::Shader::FRAGMENT);

		spriteVer.CompileFromFile("Assets/sprite.vert");
		spriteVer.CompileFromFile("Assets/sprite.frag");

		

		m_genericShader = std::make_shared<cage::Generic3DShader>(spriteVer, spriteFrag);
	}


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

void Editor::Update(float dt)
{

}

void Editor::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}