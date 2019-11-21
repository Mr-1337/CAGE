#pragma once

#include <utility>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "../CAGE/Core/GameState.hpp"
#include "../../../CAGE/Graphics/Camera/Camera.hpp"
#include "../../../CAGE/Graphics/Models/Model.hpp"
#include "../MenuButton.hpp"

class Lobby : public cage::GameState
{
public:

	Lobby(std::pair<int, int> size);

	void ProcessEvents() override;
	void Update(float delta) override;
	void Draw() override;

private:
	cage::ui::UIElement m_root;

	int m_selection;
	bool m_rotating;
	float m_theta;
	float m_rotationVel;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_timer;
	const std::chrono::duration<double, std::ratio<1, 1000>> WAIT_TIME;

	cage::Camera* m_camera;
	Mix_Chunk* m_sound;
	cage::Generic3DShader* m_shader;
	cage::Model shrek, thanos, vader, cube, car;
	std::shared_ptr<cage::SpriteShader> m_spriteShader;

};