#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 norm;

layout (binding = 0) uniform Matrices
{
	uniform mat4 u_projection;
	uniform mat4 u_view;
	uniform mat4 u_model;
} ubo;

layout(location = 0) out vec4 pos_o;
layout(location = 1) out vec3 pos_world_o;
layout(location = 2) out vec3 norm_o;
layout(location = 3) out vec2 uv_o;

void main()
{

	pos_o = (ubo.u_projection * ubo.u_view * ubo.u_model * vec4(pos, 1.0));
		
	pos_world_o = (ubo.u_model * vec4(pos, 1.0)).xyz;
	norm_o = (ubo.u_model * vec4(norm, 0.0)).xyz;
	uv_o = vec2(uv.x, uv.y);
	gl_Position = pos_o;
}