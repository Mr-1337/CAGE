#pragma once

#include "ShaderProgram.hpp"

namespace cage
{
	class Generic3DShader : public ShaderProgram
	{
	public:
		uniforms::Mat4Uniform* Model, * View, * Projection;

		Generic3DShader(const Shader& verShader, const Shader& fragShader) :
		ShaderProgram(verShader, fragShader)
		{
			Model = new uniforms::Mat4Uniform(GetID(), "u_model");
			View = new uniforms::Mat4Uniform(GetID(), "u_view");
			Projection = new uniforms::Mat4Uniform(GetID(), "u_projection");
		}
	};
}