#version 460 core

out vec4 o_color;
in vec3 o_uv;

uniform samplerCube u_skybox;

void main()
{
	o_color = texture(u_skybox, o_uv);
}