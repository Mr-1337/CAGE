#pragma once

#include "ShaderProgram.hpp"

namespace cage
{
	class SpriteShader : public ShaderProgram
	{
	public:
		uniforms::Mat4Uniform *Model, *Projection;
		uniforms::Vec2Uniform* SpriteSize;
		uniforms::BoolUniform* Textured;
		uniforms::Vec4Uniform* Color;

		SpriteShader(const Shader& verShader, const Shader& fragShader) :
			ShaderProgram(verShader, fragShader)
		{
			Model = new uniforms::Mat4Uniform(GetID(), "u_model");
			SpriteSize = new uniforms::Vec2Uniform(GetID(), "u_spriteSize");
			Projection = new uniforms::Mat4Uniform(GetID(), "u_projection");
			Textured = new uniforms::BoolUniform(GetID(), "u_textured");
			Color = new uniforms::Vec4Uniform(GetID(), "u_color");
		}

		SpriteShader() : SpriteShader(loadShaderFromFile("Assets/Shaders/sprite.vert"), loadShaderFromFile("Assets/Shaders/sprite.frag"))
		{

		}

		~SpriteShader()
		{
			delete Projection;
			delete SpriteSize;
			delete Model;
			delete Textured;
			delete Color;
		}
	};
}