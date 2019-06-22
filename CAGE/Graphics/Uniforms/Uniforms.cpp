#include "Uniforms.hpp"
#include <GLM/glm/gtc/type_ptr.hpp>

namespace cage
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
}