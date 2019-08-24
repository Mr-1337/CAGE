#pragma once

#include <chrono>

#include "../CAGE/CAGE.hpp"
#include "../CAGE/IO/MeshLoader.hpp"
#include "../CAGE/Graphics/ShaderProgram/ShaderProgram.hpp"
#include "../CAGE/Graphics/Models/Model.hpp"
#include "../CAGE/Graphics/Camera/Camera.hpp"

class STR : public cage::Game
{
public:
	STR(int argc, char** argv);
	~STR();
	void Run() override;
private:
	void processEvents(SDL_Event& e);
	void update(float delta);
	void draw();

	std::vector<cage::Vertex3UVNormal> genTerrain();

	cage::Mesh<cage::Vertex3UVNormal> world;
	cage::Model shrek;
	bool m_running;

	cage::Shader verShader, fragShader;
	cage::ShaderProgram* program;
	cage::Camera *camera;

	SDL_GameController* controller;

};