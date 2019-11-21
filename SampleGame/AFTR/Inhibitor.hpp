#pragma once

#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../CAGE/Graphics/Models/Model.hpp"
#include "../CAGE/Graphics/Uniforms/Uniforms.hpp"

class Inhibitor : public cage::Model
{
	float m_time = 0.f;
public:
	Inhibitor() = default;
	~Inhibitor() = default;

	cage::uniforms::Mat4Uniform* model = nullptr;

	void SetTime(float time) { m_time = time; }

	void Draw() override
	{
		m_meshes[1].Draw();
		model->value *= glm::rotate(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.f, 0.3 * std::sin(m_time), 0.f)), m_time, glm::vec3(0.f, 1.0f, 0.f));
		model->ForwardToShader();
		m_meshes[0].Draw();
	}
};