#pragma once

#include <GLM/glm/glm.hpp>

class Player
{
	const glm::vec3 colors[8] = {
		{ 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f },
		{ 1.0f, 0.5f, 0.0f },
		{ 0.5f, 0.5f, 0.5f }
	};
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