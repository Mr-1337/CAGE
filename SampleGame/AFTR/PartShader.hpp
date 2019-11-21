#pragma once
#include "../../CAGE/Graphics/Uniforms/Uniforms.hpp"
#include "../../CAGE/Graphics/ShaderProgram/Generic3DShader.hpp"

namespace aftr
{

	class PartShader : public cage::Generic3DShader
	{
	public:
		cage::uniforms::IntUniform* Mode;
		inline PartShader(const cage::Shader& verShader, const cage::Shader& fragShader) : Generic3DShader(verShader, fragShader)
		{
			Mode = new cage::uniforms::IntUniform(GetID(), "u_mode");
		}
	};
}