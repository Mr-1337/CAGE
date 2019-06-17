#pragma once

#include "../Shaders/Shader.hpp"
#include "../Uniforms/Uniforms.hpp"

namespace cage
{
	class ShaderProgram
	{
	public:

		ShaderProgram(const Shader& vertexShader, const Shader& fragmentShader);
		~ShaderProgram();

		// Make this the active ShaderProgram in the OpenGL context
		void Use();

		inline unsigned int GetID() const { return m_id; };
		Mat4Uniform *Model, *View, *Projection;

	private:
		unsigned int m_id;
	};
}