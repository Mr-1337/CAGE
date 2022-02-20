#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 norm;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec4 pos_o;
out vec3 pos_world_o;
out vec3 norm_o;
out vec2 uv_o;

void main()
{
	pos_o = (u_projection * u_view * u_model * vec4(pos, 1.0));
	pos_world_o = (u_model * vec4(pos, 1.0)).xyz;
	norm_o = norm;
	uv_o = vec2(uv.x, 1.0 - uv.y);
	gl_Position = pos_o;
}