#include "Uniforms.hpp"
#include <GLM/glm/gtc/type_ptr.hpp>

namespace cage::uniforms
{

	/////////////////////////////////////////////////////////////
	/////////////// INTEGER /////////////////////////////////////
	/////////////////////////////////////////////////////////////

	IntUniform::IntUniform(unsigned int program, const std::string& name) 
		: Uniform<int>(program, name)
	{

	}

	void IntUniform::ForwardToShader()
	{
		glUniform1i(m_location, value);
	}

	/////////////////////////////////////////////////////////////
	/////////////// BOOL ////////////////////////////////////////
	/////////////////////////////////////////////////////////////

	BoolUniform::BoolUniform(unsigned int program, const std::string& name)
		: Uniform<bool>(program, name)
	{
		value = false;
	}

	void BoolUniform::ForwardToShader()
	{
		glUniform1i(m_location, value);
	}


	/////////////////////////////////////////////////////////////
	///////////////  FLOAT  /////////////////////////////////////
	/////////////////////////////////////////////////////////////

	FloatUniform::FloatUniform(unsigned int program, const std::string& name)
		: Uniform<float>(program, name)
	{

	}

	void FloatUniform::ForwardToShader()
	{
		glUniform1f(m_location, value);
	}

	/////////////////////////////////////////////////////////////
	///////////////   Mat4  /////////////////////////////////////
	/////////////////////////////////////////////////////////////

	Mat4Uniform::Mat4Uniform(unsigned int program, const std::string& name)
		: Uniform<glm::mat4>(program, name)
	{
		value = glm::identity<glm::mat4>();
	}

	void Mat4Uniform::ForwardToShader()
	{
		glUniformMatrix4fv(m_location, 1, GL_FALSE, glm::value_ptr(value));
	}

	/////////////////////////////////////////////////////////////
	///////////////   Vec2  /////////////////////////////////////
	/////////////////////////////////////////////////////////////

	Vec2Uniform::Vec2Uniform(unsigned int program, const std::string& name)
		: Uniform<glm::vec2>(program, name)
	{
		value = glm::vec2{ 0.f };
	}

	void Vec2Uniform::ForwardToShader()
	{
		glUniform2fv(m_location, 1, glm::value_ptr(value));
	}

	/////////////////////////////////////////////////////////////
	///////////////   Vec3  /////////////////////////////////////
	/////////////////////////////////////////////////////////////

	Vec3Uniform::Vec3Uniform(unsigned int program, const std::string& name)
		: Uniform<glm::vec3>(program, name)
	{
		value = glm::vec3{ 0.f };
	}

	void Vec3Uniform::ForwardToShader()
	{
		glUniform3fv(m_location, 1, glm::value_ptr(value));
	}

	/////////////////////////////////////////////////////////////
	///////////////   Vec4  /////////////////////////////////////
	/////////////////////////////////////////////////////////////

	Vec4Uniform::Vec4Uniform(unsigned int program, const std::string& name)
		: Uniform<glm::vec4>(program, name)
	{
		value = glm::vec4{ 0.f };
	}

	void Vec4Uniform::ForwardToShader()
	{
		glUniform4fv(m_location, 1, glm::value_ptr(value));
	}
}