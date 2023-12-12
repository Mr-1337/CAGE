#version 460 core

layout (location = 0) in vec2 pos;

layout (push_constant) uniform PushConstants
{
	uniform mat4 u_projection;
	uniform mat4 u_model;
	uniform vec4 u_size;
	uniform vec4 u_color;
	uniform vec4 u_textured;
} pc;

layout (location = 0) out vec2 uv_o;

void main()
{
	uv_o = vec2(pos.x + 0.5, pos.y + 0.5);
	gl_Position = pc.u_projection * pc.u_model * (vec4(pc.u_size.x, pc.u_size.y, 0, 1.0) * vec4(pos, 0.0, 1.0));
	gl_Position.z = 0.95;
	gl_Position.y *= -1;
	//gl_Position = pc.u_projection * pc.u_model * vec4(pos, 0.0, 1.0);
}