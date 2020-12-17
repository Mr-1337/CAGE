#pragma once

#include "../../CAGE/Graphics/Models/Model.hpp"

class Car
{
private:
	cage::Model model;
public:
	Car();
	glm::vec2 heading;
	glm::vec3 velocity;
	glm::vec3 position;
	void Update(float delta);

	inline void Draw() { model.Draw(); }
};