#pragma once

#include "ShaderProgram.hpp"

namespace cage
{
	class Generic3DShader : public ShaderProgram
	{
	public:
		uniforms::Mat4Uniform* Model, * View, * Projection;
		uniforms::Vec4Uniform* Tint;

		Generic3DShader(const Shader& verShader, const Shader& fragShader) :
		ShaderProgram(verShader, fragShader)
		{
			std::cout << "Generic 3D Shader created" << std::endl;
			Model = new uniforms::Mat4Uniform(GetID(), "u_model");
			View = new uniforms::Mat4Uniform(GetID(), "u_view");
			Projection = new uniforms::Mat4Uniform(GetID(), "u_projection");
			Tint = new uniforms::Vec4Uniform(GetID(), "u_tint");
		}

		~Generic3DShader()
		{
			delete Tint;
			delete Projection;
			delete View;
			delete Model;
		}
	};
}