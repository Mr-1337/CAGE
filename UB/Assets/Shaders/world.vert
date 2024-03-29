#version 460 core

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec2 i_uv;

uniform mat4 u_viewProjection;
uniform mat4 u_model;

out vec2 o_uv;

void main()
{
	o_uv = i_uv;
	gl_Position = u_viewProjection * u_model * vec4(i_pos, 1.0);
}