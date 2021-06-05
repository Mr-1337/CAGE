#pragma once

#include <Glad/glad/glad.h>
#include <fstream>
#include <string>
#include <sstream>

namespace cage
{
	class Shader
	{
	public:

		enum class ShaderType
		{
			VERTEX = GL_VERTEX_SHADER,
			FRAGMENT = GL_FRAGMENT_SHADER
		};

		Shader(ShaderType type);
		Shader(const Shader& other) = delete;
		Shader(Shader&& other);

		~Shader();

		void CompileFromFile(const std::string& path);
		void CompileFromSrcString(const std::string& src);
		inline unsigned int GetID() const { return m_id; }

	private:

		void compile(const char* src);

		// The OpenGL id or "name" for this shader object
		unsigned int m_id;

		// Keeps track of if we own the data before freeing
		bool m_ownsShader;

	};
}