#version 460 core

layout (binding = 0) uniform sampler2D u_texture;
layout (push_constant) uniform PushConstants
{
	uniform mat4 u_projection;
	uniform mat4 u_model;
	uniform vec4 u_size;
	uniform vec4 u_color;
	uniform vec4 u_textured;
} pc;

layout (location = 0) in vec2 uv_o;
layout (location = 0) out vec4 colorOut;

void main()
{
	//colorOut = vec4(2.f * uv_o, 0.0, 1.0);
	if (pc.u_textured.x == 1)
		colorOut = texture(u_texture, uv_o);
		//colorOut = vec4(uv_o, 0.0, 1.0);
		//colorOut = pc.u_color;
	else
		colorOut = pc.u_color;
}