#version 460 core

in vec2 o_uv;

uniform sampler2D u_texture;
uniform bool u_textured;
uniform vec4 u_color;

out vec4 colorOut;

void main()
{
	//colorOut = vec4(1.0, o_uv.y, 0.0, 1.0);
	if (u_textured)
		colorOut = texture(u_texture, o_uv);
	else
		colorOut = u_color;
}