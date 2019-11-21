#pragma once

#include <vector>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "../CAGE/Audio/AudioSource.hpp"
#include "../CAGE/Graphics/Models/Model.hpp"
#include "../Car.hpp"
#include "../CAGE/Graphics/Camera/Camera.hpp"
#include "../CAGE/Graphics/UI/Button.hpp"
#include "../../../CAGE/Core/GameState.hpp"
#include "../Perlin.hpp"


class Sandbox : public cage::GameState
{
public:

	Sandbox(std::pair<int, int> size);

	void ProcessEvents() override;
	void Update(float delta) override;
	void Draw() override;

private:

	std::vector<cage::Vertex3UVNormal> genTerrain();

	cage::Mesh<cage::Vertex3UVNormal> world;
	cage::Model shrek, thanos, vader;
	Car car;

	cage::Shader verShader, fragShader, spriteVerShader, spriteFragShader;
	cage::Generic3DShader* program;
	std::shared_ptr<cage::SpriteShader> spriteProgram;
	cage::Camera* camera;

	Mix_Chunk* music;

	Mix_Chunk *mus1, *mus2, *mus3;

	cage::AudioSource s1, s2, s3, s4;
	cage::Listener listener;

	cage::ui::UIElement m_root;
	std::shared_ptr<cage::ui::UIElement> m_speedometer, m_needle, m_fpsCounter, m_menuOverlay;

	TTF_Font* m_font;
	const SDL_Color FONT_COLOR = { 255, 0, 0, 255 };

	unsigned int m_eboid;
};