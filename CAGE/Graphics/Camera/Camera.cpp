#include "Camera.hpp"

namespace cage
{
	Camera::Camera(ShaderProgram& program) : m_program(program)
	{
		cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		cameraUp = { 0.f, 1.f, 0.f };
		cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	void Camera::SetProjection(const glm::mat4& projection)
	{
		m_projection = projection;
		m_program.Projection->value = m_projection;
		m_program.Projection->ForwardToShader();
	}

	void Camera::Apply()
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		if (!m_thirdPerson)
		{
			front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
			front.y = sin(glm::radians(pitch));
			front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
			cameraFront = glm::normalize(front);
			right = glm::normalize(glm::cross(cameraFront, { 0.f, 1.0f, 0. }));
			m_program.View->value = glm::lookAt(m_position, m_position + cameraFront, cameraUp);
		}
		else
		{
			glm::vec3 offset;
			offset.x = 5 * std::cos(yaw/20.f);
			offset.z = 5 * std::sin(yaw/20.f);
			offset.y = 5 * -std::sin(pitch / 20.f);

			offset += glm::vec3{ 0.f, 0.f, 3.f };
			m_program.View->value = glm::lookAt(m_position + offset, m_position + glm::vec3{ 0.0f, 0.0f, 3.f }, cameraUp);
		}
		m_program.View->ForwardToShader();
	}

	Camera& Camera::Move(const glm::vec3& translation)
	{
		m_position += translation;
		return *this;
	}

	Camera& Camera::MoveForward(float amount)
	{
		m_position += cameraFront * amount;
		return *this;
	}

	Camera& Camera::MoveLeftRight(float amount)
	{
		m_position += right * amount;
		return *this;
	}

	Camera& Camera::Rotate(const glm::vec3& axis, float angle)
	{
		return *this;
	}

	Camera& Camera::Rotate(float yaw, float pitch, float roll)
	{
		return *this;
	}

	Camera& Camera::SetOrientation(float yaw, float pitch, float roll)
	{
		return *this;
	}
}