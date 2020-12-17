#pragma once

#include <utility>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../CAGE/Core/GameState.hpp"

#include "../../CAGE/Graphics/UI/GridLayout.hpp"
#include "../MenuButton.hpp"

class ShrekMenu : public cage::GameState
{
	using Ref = std::shared_ptr<cage::ui::UIElement>;
public:

	ShrekMenu(cage::Game&);

	void OnRevealed() override;
	void OnHidden() override;

	void ProcessEvents() override;
	void Update(float delta) override;
	void Draw() override;

private:
	cage::ui::UIElement m_root;
	std::shared_ptr<cage::SpriteShader> m_spriteShader;
	std::shared_ptr<cage::ui::LayoutGroup> m_buttonGroup;

};