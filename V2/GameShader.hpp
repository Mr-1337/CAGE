#pragma once

#include <Graphics/ShaderProgram/ShaderProgram.hpp>

namespace v2
{
	class GameShader : public cage::ShaderProgram
	{
		GameShader(const cage::Shader& vertex, const cage::Shader& fragment) : cage::ShaderProgram(vertex, fragment)
		{

		}
	public:

		cage::uniforms::Mat4Uniform* Projection;

		GameShader() : GameShader(loadShaderFromFile("Assets/Shaders/game.vert"), loadShaderFromFile("Assets/Shaders/game.frag"))
		{
			Projection = new cage::uniforms::Mat4Uniform(GetID(), "u_projection");
		}

		~GameShader()
		{
			delete Projection;
		}

	};
}