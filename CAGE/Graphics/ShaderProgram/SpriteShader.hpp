#pragma once

#include "ShaderProgram.hpp"

namespace cage
{
	class SpriteShader : public ShaderProgram
	{
	public:
		uniforms::Mat4Uniform *Model, *Projection;
		uniforms::Vec2Uniform* SpriteSize;

		SpriteShader(const Shader& verShader, const Shader& fragShader) :
			ShaderProgram(verShader, fragShader)
		{
			Model =      new uniforms::Mat4Uniform(GetID(), "u_model");
			SpriteSize = new uniforms::Vec2Uniform(GetID(), "u_spriteSize");
			Projection = new uniforms::Mat4Uniform(GetID(), "u_projection");
		}

	};
}