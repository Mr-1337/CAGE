#pragma once

#include <array>
#include <GLM/glm/gtc/quaternion.hpp>

#include "../CAGE/CAGE.hpp"
#include "../CAGE/Graphics/ShaderProgram/Generic3DShader.hpp"
#include "../CAGE/Graphics/Models/Model.hpp"
#include "../CAGE/Graphics/Camera/Camera.hpp"
#include "Inhibitor.hpp"
#include "PartShader.hpp"
#include "Network.hpp"
#include "Cross.hpp"
#include "Well.hpp"
#include "Pipe.hpp"

/* Definitely not the final implementation, don't take this too seriously! */

class AFTR : public cage::Game
{
	bool m_running;

	cage::Model m_tube, m_cross, m_cube, m_shrek;

	Inhibitor m_inhibitor;
	std::array<cage::Model*, 5> m_parts;

	std::shared_ptr<aftr::PartShader> m_shader;
	std::unique_ptr<cage::Camera> m_camera;

	glm::vec3 m_cubePosition, placeSpot;

	int m_selection = 0;
	aftr::Directions m_orientation;

	const int WORLD_SIZE = 50;
	std::vector<cage::Model*> world;
	aftr::Network m_network;

public:
	AFTR(int argc, char** argv);
	~AFTR();

	void Run() override;

private:
	void update(double dt);
	void draw();
	void handleEvents();

	glm::vec4 oriToRot(aftr::Directions);

	aftr::ComponentBase* partFactory(int partID);

	double totalTime;

	void drawGrid();

};