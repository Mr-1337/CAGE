#pragma once

#include <GLM/glm/gtc/matrix_transform.hpp>

#include "../../../CAGE/Core/GameState.hpp"
#include "../../../CAGE/Graphics/UI/CheckBox.hpp"
#include "../../../CAGE/Graphics/ShaderProgram/Generic3DShader.hpp"
#include "../../../CAGE/Graphics/Camera/Camera.hpp"

#include "../MenuButton.hpp"
#include "../Track/Track.hpp"

class Editor : public cage::GameState
{
public:
	Editor(cage::Game& game, std::pair<int, int> size);

	void OnRevealed() override;
	void OnHidden() override;

	void ProcessEvents() override;
	void Update(float dt) override;
	void Draw() override;
private:
	cage::ui::UIElement m_root;

	std::shared_ptr<cage::Generic3DShader> m_genericShader;
	std::shared_ptr<cage::SpriteShader> m_spriteShader;

	bool m_wireframe;
	int m_selectedNode;

	std::shared_ptr<cage::Camera> m_camera;
	cage::Model shrek, thanos;
	Track m_track;
};