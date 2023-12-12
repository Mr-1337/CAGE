#pragma once

// TODO: Quaternions instead of Euler Angles!!!
#include <GLM/glm/gtc/matrix_transform.hpp>

namespace cage
{
	namespace graphics
	{
		class Camera
		{

			glm::vec3 cameraTarget;
			glm::vec3 cameraDirection;
			glm::vec3 cameraUp;
			glm::vec3 cameraFront;
			glm::vec3 front;
			glm::vec3 right;

			glm::vec3 m_position;
			glm::mat4 m_projection, m_view;

			std::pair<int, int> m_winSize;

			bool m_thirdPerson = false;
			bool m_ignoreYaw = false;
			float m_fov;
		public:

			float yaw = 0.f, pitch = 0.f;
			Camera();
			~Camera() = default;

			// Returns the combined view and projection matrices
			glm::mat4 GetMatrix();
			inline glm::mat4 GetViewMatrix() { return m_view; };
			Camera& Move(const glm::vec3& translation);
			Camera& Rotate(const glm::vec3& axis, float angle);
			Camera& Rotate(float yaw, float pitch, float roll);

			Camera& MoveForward(float amount);
			Camera& MoveLeftRight(float amount);

			inline void IgnoreYaw(bool ignores) { m_ignoreYaw = ignores; }

			// Set the position in world space
			inline Camera& SetPosition(const glm::vec3& position) { m_position = position; return *this; }
			// Retreives the position in world space
			inline glm::vec3 GetPosition() const { return m_position; }
			inline glm::vec3 GetRight() const { return right; }
			Camera& SetOrientation(float yaw, float pitch, float roll);
			inline glm::vec3 GetFront() const { return cameraFront; }
			inline void ToggleMode() { m_thirdPerson = !m_thirdPerson; }
			inline void SetWinSize(std::pair<int, int> size) { m_winSize = size; recalcProjection(); }
			void recalcProjection();
			// Sets the projection matrix for this camera
			void SetProjection(const glm::mat4& projection);
			void SetFOV(float fov);
		};
	}
}