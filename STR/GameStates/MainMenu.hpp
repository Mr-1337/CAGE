#pragma once

#include <utility>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../CAGE/Core/GameState.hpp"

#include "../MenuButton.hpp"

class MainMenu : public cage::GameState
{
	using Ref = std::shared_ptr<cage::ui::UIElement>;
public:

	MainMenu(cage::Game&, std::pair<int, int> size);

	void OnRevealed() override;
	void OnHidden() override;

	void ProcessEvents() override;
	void Update(float delta) override;
	void Draw() override;
	void Add(Ref thingy);

private:
	cage::ui::UIElement m_root;
	std::shared_ptr<cage::SpriteShader> m_spriteShader;

};