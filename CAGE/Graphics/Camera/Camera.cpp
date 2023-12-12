#include "Camera.hpp"

namespace cage
{
	namespace graphics
	{
		Camera::Camera()
		{
			m_projection = glm::identity<glm::mat4>();
			m_view = glm::identity<glm::mat4>();
			cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
			cameraUp = { 0.f, 1.f, 0.f };
			cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
			m_position = { 0.f, 0.0, 0.0 };
			m_fov = 90.f;
			yaw = 0;
		}

		glm::mat4 Camera::GetMatrix()
		{
			if (pitch > 89.0f * ((float)m_thirdPerson * (1.0f / 3.5f) + !m_thirdPerson))
				pitch = 89.0f * ((float)m_thirdPerson * (1.0f / 3.5f) + !m_thirdPerson);
			if (pitch < -89.0f * ((float)m_thirdPerson * (1.0f / 3.5f) + !m_thirdPerson))
				pitch = -89.0f * ((float)m_thirdPerson * (1.0f / 3.5f) + !m_thirdPerson);
			if (!m_thirdPerson)
			{
				front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
				front.y = sin(glm::radians(pitch));
				front.z = -cos(glm::radians(pitch)) * sin(glm::radians(yaw));
				cameraFront = glm::normalize(front);
				right = glm::normalize(glm::cross(cameraFront, { 0.f, 1.0f, 0. }));
				m_view = glm::lookAtRH(m_position, m_position + cameraFront, cameraUp);
			}
			else
			{
				glm::vec3 offset;
				offset.x = 5 * std::cos(yaw / 20.f);
				offset.z = 5 * std::sin(yaw / 20.f);
				offset.y = 5 * -std::sin(pitch / 20.f);

				offset += glm::vec3{ 0.f, 0.f, 3.f };
				m_view = glm::lookAtRH(m_position + offset, m_position + glm::vec3{ 0.0f, 0.0f, 3.f }, cameraUp);
			}
			return m_projection * m_view;
		}

		void Camera::SetProjection(const glm::mat4& projection)
		{
			m_projection = projection;
		}

		void Camera::recalcProjection()
		{
			m_projection = glm::perspectiveFovRH_ZO(glm::radians(m_fov), static_cast<float>(m_winSize.first), static_cast<float>(m_winSize.second), 0.01f, 1000.0f);
			m_projection[1][1] *= -1;
		}

		Camera& Camera::Move(const glm::vec3& translation)
		{
			m_position += translation;
			return *this;
		}

		Camera& Camera::MoveForward(float amount)
		{
			if (m_ignoreYaw)
			{
				Move(glm::normalize(glm::vec3{ front.x / std::cos(glm::radians(pitch)), 0.f, front.z / std::cos(glm::radians(pitch)) }) * amount);
			}
			else
				Move(cameraFront * amount);
			return *this;
		}

		Camera& Camera::MoveLeftRight(float amount)
		{
			Move(right * amount);
			return *this;
		}

		Camera& Camera::Rotate(const glm::vec3& axis, float angle)
		{
			return *this;
		}

		Camera& Camera::Rotate(float yaw, float pitch, float roll)
		{
			this->yaw += yaw;
			this->pitch += pitch;
			//this->roll += roll;
			return *this;
		}

		Camera& Camera::SetOrientation(float yaw, float pitch, float roll)
		{
			return *this;
		}
	}
}