#pragma once

#include <GLM/glm/glm.hpp>
#include <iostream>
#include <Glad/glad/glad.h>

namespace cage::uniforms
{
	template <class T>
	class Uniform
	{
	public:

		Uniform(unsigned int program, const std::string& name) : m_location(-1), m_program(program), m_name(name)
		{
			m_location = glGetUniformLocation(m_program, m_name.c_str());
		}

		virtual void ForwardToShader() = 0;
		T value;

	protected:
		// A handle to the location of this Uniform in the shader progarm
		int m_location;
		// The OpenGL program this uniform belongs to
		const unsigned int m_program;
		// The name of this uniform
		const std::string& m_name;
	};

	// Integer

	class IntUniform : public Uniform<int>
	{
	public:
		IntUniform(unsigned int program, const std::string& name);
		void ForwardToShader() override;
	};

	// Float

	class FloatUniform : public Uniform<float>
	{
	public:
		FloatUniform(unsigned int program, const std::string& name);
		void ForwardToShader() override;
	};

	// Mat4

	class Mat4Uniform : public Uniform<glm::mat4>
	{
	public:
		Mat4Uniform(unsigned int program, const std::string& name);
		void ForwardToShader() override;
	};

	// Vec2

	class Vec2Uniform : public Uniform<glm::vec2>
	{
	public:
		Vec2Uniform(unsigned int program, const std::string& name);
		void ForwardToShader() override;
	};

	// Vec3

	class Vec3Uniform : public Uniform<glm::vec3>
	{
	public:
		Vec3Uniform(unsigned int program, const std::string& name);
		void ForwardToShader() override;
	};
}