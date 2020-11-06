#include <SDL2/SDL_mixer.h>

#include "../../../CAGE/Core/GameState.hpp"
#include "../../../CAGE/Graphics/ShaderProgram/SpriteShader.hpp"

#include "../CAGE/Graphics/UI/Button.hpp"
#include "../CAGE/Graphics/UI/CheckBox.hpp"
#include "../CAGE/Graphics/UI/Text.hpp"
#include "../CAGE/IO/Networking/Endpoint.hpp"

#pragma once

class UITest : public cage::GameState
{
public:
	UITest(cage::Game& game, std::pair<int, int> size);

	void OnRevealed() override;
	void OnHidden() override;

	void ProcessEvents() override;
	void Update(float dt) override;
	void Draw() override;

private:
	std::shared_ptr<cage::SpriteShader> m_spriteShader;
	cage::ui::UIElement m_root;
	std::shared_ptr<cage::ui::Button> b1, b2, b3, b4;
	std::shared_ptr<cage::ui::Text> message;
	std::shared_ptr<cage::ui::CheckBox> c1, c2, c3;

	Mix_Chunk *s1, *s2, *s3, *s4;
};