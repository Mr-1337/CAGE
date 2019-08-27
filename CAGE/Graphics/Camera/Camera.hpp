#pragma once

// TODO: Quaternions instead of Euler Angles!!!
#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../ShaderProgram/Generic3DShader.hpp"

namespace cage
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
		glm::mat4 m_projection;
		Generic3DShader& m_program;

		bool m_thirdPerson = false;
	public:

		float yaw = 0.f, pitch = 0.f;

		Camera(Generic3DShader& program);
		~Camera() = default;

		Camera& Move(const glm::vec3& translation);
		Camera& Rotate(const glm::vec3& axis, float angle);
		Camera& Rotate(float yaw, float pitch, float roll);

		Camera& MoveForward(float amount);
		Camera& MoveLeftRight(float amount);
		
		// Set the position in world space
		inline Camera& SetPosition(const glm::vec3& position) { m_position = position; return *this; }
		// Retreives the position in world space
		inline glm::vec3 GetPosition() { return m_position; }
		Camera& SetOrientation(float yaw, float pitch, float roll);

		inline void ToggleMode() { m_thirdPerson = !m_thirdPerson; }

		// Sets the projection matrix for this camera
		void SetProjection(const glm::mat4& projection);

		// Applies this camera to the scene
		void Apply();
	};
}