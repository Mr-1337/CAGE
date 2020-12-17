#version 460 core

layout (location = 0) in vec3 pos;

out vec3 o_uv;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
	o_uv = pos;
	vec4 temp = u_view * vec4(pos, 0.0);
	temp.w = 1.0;
	gl_Position = u_projection * temp;
}