#version 460 core

in vec2 uv_o;

uniform sampler2D u_texture;

out vec4 colorOut;

void main()
{
	//colorOut = vec4(uv_o, 0.0, 1.0);
	colorOut = texture(u_texture, uv_o);
}