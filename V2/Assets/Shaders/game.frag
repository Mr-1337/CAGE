#version 460 core

in vec2 o_uv;

out vec4 colorOut;

uniform sampler2D u_texture;

void main()
{
	colorOut = texture(u_texture, o_uv);
}