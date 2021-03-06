#include "ShaderProgram.hpp"
#include <iostream>
#include <filesystem>

namespace cage
{
	ShaderProgram::ShaderProgram(const Shader& vertexShader, const Shader& fragmentShader)
	{
		m_id = glCreateProgram();

		glAttachShader(m_id, vertexShader.GetID());
		glAttachShader(m_id, fragmentShader.GetID());

		glLinkProgram(m_id);

		// Check for errors
		GLint success = 0;
		glGetProgramiv(m_id, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

			char* log = new char[maxLength];
			glGetProgramInfoLog(m_id, maxLength, &maxLength, log);

			std::cout << "Shader program linking failed! Error: " << log << std::endl;

			glDeleteProgram(m_id);

		}

		glDetachShader(m_id, vertexShader.GetID());
		glDetachShader(m_id, fragmentShader.GetID());


	}

	ShaderProgram::ShaderProgram(const Shader& vertexShader, const Shader& geometryShader, const Shader& fragmentShader)
	{
		m_id = glCreateProgram();

		glAttachShader(m_id, vertexShader.GetID());
		glAttachShader(m_id, geometryShader.GetID());
		glAttachShader(m_id, fragmentShader.GetID());

		glLinkProgram(m_id);

		// Check for errors
		GLint success = 0;
		glGetProgramiv(m_id, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

			char* log = new char[maxLength];
			glGetProgramInfoLog(m_id, maxLength, &maxLength, log);

			std::cout << "Shader program linking failed! Error: " << log << std::endl;

			glDeleteProgram(m_id);

		}

		glDetachShader(m_id, vertexShader.GetID());
		glDetachShader(m_id, geometryShader.GetID());
		glDetachShader(m_id, fragmentShader.GetID());

	}

	Shader ShaderProgram::loadShaderFromFile(const std::string& path) const
	{
		Shader::ShaderType type;
		std::filesystem::path p(path);
		auto ext = p.extension();
		if (ext == ".vert")
			type = Shader::ShaderType::VERTEX;
		else if (ext == ".frag")
			type = Shader::ShaderType::FRAGMENT;

		Shader shader(type);
		shader.CompileFromFile(path);
		return shader;
	}

	ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(m_id);
	}

	void ShaderProgram::Use()
	{
		glUseProgram(m_id);
	}
}