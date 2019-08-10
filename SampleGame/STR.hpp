#pragma once

#include <chrono>

#include "../CAGE/CAGE.hpp"
#include "../CAGE/IO/MeshLoader.hpp"
#include "../CAGE/Graphics/VertexArrays/VertexArray.hpp"
#include "../CAGE/Graphics/ShaderProgram/ShaderProgram.hpp"
#include "../CAGE/IO/Networking/Endpoint.hpp"
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

	std::vector<cage::Vertex3UVNormal> shrek;
	cage::VertexBuffer<cage::Vertex3UVNormal> vbo, worldvbo;
	cage::VertexArray<cage::Vertex3UVNormal> *vao, *vao2;
	bool m_running;

	cage::Shader verShader, fragShader;
	cage::ShaderProgram* program;
	cage::networking::Endpoint us, *them;
	cage::Camera *camera;

};