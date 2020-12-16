#pragma once

#include "ShaderProgram.hpp"

namespace cage
{
	class SkyboxShader : public ShaderProgram
	{
	public:
		uniforms::Mat4Uniform * View, * Projection;

		SkyboxShader(const Shader& verShader, const Shader& fragShader) :
			ShaderProgram(verShader, fragShader)
		{
			std::cout << "Skybox Shader created" << std::endl;
			View = new uniforms::Mat4Uniform(GetID(), "u_view");
			Projection = new uniforms::Mat4Uniform(GetID(), "u_projection");
		}

		SkyboxShader() : SkyboxShader(loadShaderFromFile("Assets/skybox.vert"), loadShaderFromFile("Assets/skybox.frag"))
		{

		}

		~SkyboxShader()
		{
			delete Projection;
			delete View;
		}
	};
}