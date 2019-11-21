#pragma once

#include <utility>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../CAGE/Core/GameState.hpp"

#include "../MenuButton.hpp"

class MainMenu : public cage::GameState
{
public:

	MainMenu(std::pair<int, int> size);

	void ProcessEvents() override;
	void Update(float delta) override;
	void Draw() override;

private:
	cage::ui::UIElement m_root;
	std::shared_ptr<cage::SpriteShader> m_spriteShader;

};