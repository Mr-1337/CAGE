#version 460 core

layout (location = 0) in vec2 i_pos;
layout (location = 1) in vec2 i_uv;

uniform mat4 u_viewProjection;

out vec2 o_uv;

void main()
{
	o_uv = i_uv;
	gl_Position = u_viewProjection * vec4(i_pos, 0, 1.0);
}