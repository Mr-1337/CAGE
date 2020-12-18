#pragma once

#include <GLM/glm/glm.hpp>

class Player
{
	const glm::vec3 colors = {
		{ 0.0, 0.0, 1.0 },
		{ 1.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0 },
		{ 1.0, 1.0, 0.0 },
		{ 0.0, 1.0, 1.0 },
		{ 1.0, 0.0, 1.0 },
		{ 1.0, 0.5, 0.0 },
		{ 0.5, 0.5, 0.5 }
	}
public:
	int PlayerID;

	Player(int id) : PlayerID(id)
	{

	}

	glm::vec3 GetColor()
	{
		return colors[PlayerID];
	}

};