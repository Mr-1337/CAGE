#pragma once

#include "../CAGE/Graphics/ShaderProgram/ShaderProgram.hpp"

namespace ub
{
	class WorldShader : public cage::ShaderProgram
	{
	public:
		cage::uniforms::Mat4Uniform* m_ViewProjection;
		cage::uniforms::BoolUniform* m_Textured;
		cage::uniforms::Vec4Uniform* m_Color;

		WorldShader(const cage::Shader& vertex, const cage::Shader& fragment) : cage::ShaderProgram(vertex, fragment)
		{
			m_ViewProjection = new cage::uniforms::Mat4Uniform(GetID(), "u_viewProjection");
			m_Textured = new cage::uniforms::BoolUniform(GetID(), "u_textured");
			m_Color = new cage::uniforms::Vec4Uniform(GetID(), "u_color");
		}

		WorldShader() : WorldShader(loadShaderFromFile("Assets/Shaders/world.vert"), loadShaderFromFile("Assets/Shaders/world.frag"))
		{
		}

		~WorldShader()
		{
			delete m_ViewProjection;
			delete m_Textured;
			delete m_Color;
		}

	};
}