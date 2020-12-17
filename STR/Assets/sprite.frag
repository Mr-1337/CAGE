#version 460 core

in vec2 uv_o;

uniform sampler2D u_texture;
uniform bool u_textured;
uniform vec4 u_color;

out vec4 colorOut;

void main()
{
	//colorOut = vec4(2.f * uv_o, 0.0, 1.0);
	if (u_textured)
		colorOut = texture(u_texture, uv_o);
	else
		colorOut = u_color;
}