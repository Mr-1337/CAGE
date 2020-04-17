#pragma once

#include <GLM/glm/gtc/matrix_transform.hpp>

#include "../../../CAGE/Core/GameState.hpp"
#include "../../../CAGE/Graphics/UI/CheckBox.hpp"
#include "../../../CAGE/Graphics/ShaderProgram/Generic3DShader.hpp"

#include "../MenuButton.hpp"

class Editor : public cage::GameState
{
public:
	Editor(std::pair<int, int> size);
	void ProcessEvents() override;
	void Update(float dt) override;
	void Draw() override;
private:
	cage::ui::UIElement m_root;
	std::shared_ptr<cage::Generic3DShader> m_genericShader;
	std::shared_ptr<cage::SpriteShader> m_spriteShader;
};