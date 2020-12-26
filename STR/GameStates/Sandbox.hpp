#pragma once

#include <vector>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SteamAudio/phonon.h>
#include <chrono>

#include "../CAGE/Graphics/Models/Model.hpp"
#include "../Car.hpp"
#include "../CAGE/Graphics/Camera/Camera.hpp"
#include "../CAGE/Graphics/UI/Button.hpp"
#include "../CAGE/Graphics/UI/Text.hpp"
#include "../../CAGE/Core/GameState.hpp"
#include "../Perlin.hpp"


class Sandbox : public cage::GameState
{
public:

	Sandbox(cage::Game& game, std::pair<int, int> size);

	void OnRevealed() override;
	void OnHidden() override;

	void ProcessEvents() override;
	void Update(float delta) override;
	void Draw() override;

private:

	int const samplingrate = 44100;
	int const framesize = 1024;

	IPLhandle context{ nullptr };
	
	IPLAudioBuffer inBuffer;
	IPLAudioBuffer outBuffer;

	IPLhandle effect, effect2;

	IPLhandle m_audioRenderer;
	IPLhandle m_envRenderer;

	float totalTime;

	float* srcBuff;

	std::vector<cage::Vertex3UVNormal> genTerrain();
	std::vector<float> audioBuffer;

	cage::Mesh<cage::Vertex3UVNormal> world;
	cage::Model shrek, thanos, vader;
	Car car;

	cage::Shader verShader, fragShader, spriteVerShader, spriteFragShader;
	cage::Generic3DShader* program;
	std::shared_ptr<cage::SpriteShader> spriteProgram;
	cage::Camera* camera;

	Mix_Chunk* music;

	Mix_Chunk *mus1, *mus2, *mus3;

	Mix_Chunk shrekSong;


	cage::ui::UIElement m_root;
	std::shared_ptr<cage::ui::UIElement> m_speedometer, m_needle, m_menuOverlay;
	std::shared_ptr<cage::ui::Text> m_fpsCounter;

	cage::Font* m_font;
	const SDL_Color FONT_COLOR = { 255, 0, 0, 255 };

	unsigned int m_eboid;
};