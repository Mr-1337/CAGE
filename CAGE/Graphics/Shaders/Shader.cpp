#include "Shader.hpp"
#include <iostream>

namespace cage
{
	Shader::Shader(ShaderType type)
	{
		m_id = glCreateShader((GLenum)type);
	}

	void Shader::CompileFromFile(const std::string& path)
	{
		std::ifstream inputFile(path);

		if (!inputFile.is_open())
		{
			throw std::runtime_error("Unable to find specified shader file!");
		}

		std::stringstream lines;
		std::string currLine;
		while (std::getline(inputFile, currLine))
		{
			lines << currLine << std::endl;
		}

		compile(lines.str().c_str());
	}

	void Shader::CompileFromSrcString(const std::string& src)
	{
		compile(src.c_str());
	}

	void Shader::compile(const char* src)
	{
		// Give OpenGL the actual source code
		glShaderSource(m_id, 1, &src, nullptr);
		glCompileShader(m_id);

		// Check for compilation errors

		GLint success = 0;
		glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint maxLength;
			glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);
			char* log = new char[maxLength];
			glGetShaderInfoLog(m_id, maxLength, &maxLength, log);

			std::cout << "Failed to compile shader! Error: " << log << std::endl;
			delete[] log;
		}
	}

	Shader::~Shader()
	{
		glDeleteShader(m_id);
	}
}