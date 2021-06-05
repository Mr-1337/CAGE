#pragma once

#include "../Shaders/Shader.hpp"
#include "../Uniforms/Uniforms.hpp"

namespace cage
{
	class ShaderProgram
	{
	public:

		ShaderProgram(const Shader& vertexShader, const Shader& fragmentShader);
		ShaderProgram(const Shader& vertexShader, const Shader& geometryShader, const Shader& fragmentShader);

		ShaderProgram(const ShaderProgram& other) = delete;
		ShaderProgram(const ShaderProgram&& other) = delete;

		virtual ~ShaderProgram();

		// Make this the active ShaderProgram in the OpenGL context
		void Use();

		inline unsigned int GetID() const { return m_id; };
	protected:

		Shader loadShaderFromFile(const std::string& path) const;

	private:
		unsigned int m_id;
	};
}