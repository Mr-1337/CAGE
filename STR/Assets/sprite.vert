#version 460 core

layout (location = 0) in vec2 pos;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform vec2 u_spriteSize;

out vec2 uv_o;

void main()
{
	uv_o = vec2(pos.x + 0.5, pos.y + 0.5);
	gl_Position = u_projection * u_model * (vec4(u_spriteSize, 0, 1.0) * vec4(pos, 0, 1.0));
}