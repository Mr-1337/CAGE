#version 460 core

layout (location = 0) in vec2 i_pos;
layout (location = 1) in vec2 i_uv;

out vec2 o_uv;

uniform mat4 u_projection;

void main()
{
	o_uv = i_uv;
	gl_Position = u_projection * vec4(i_pos, 0.0, 1.0);
}