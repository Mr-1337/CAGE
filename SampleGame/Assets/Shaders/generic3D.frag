#version 460 core

layout(location = 0) in vec4 pos_o;
layout(location = 1) in vec3 pos_world_o;
layout(location = 2) in vec3 norm_o;
layout(location = 3) in vec2 uv_o;
layout(location = 0) out vec4 colorOut;

layout(binding = 1) uniform sampler2D myTexture;

//uniform sampler2D u_texture;

void main()
{
	//float diffuse = max(dot(norm_o, normalize(-pos_world_o + vec3(200.0, 20.0, 200.0))) / (0.05 * length(-pos_world_o + vec3(200.0, 20.0, 200.0))), 0.001);
	float diffuse = max(dot(norm_o, normalize(-pos_world_o + vec3(20.0, 20.0, 20.0))), 0.01);
	//colorOut = vec4(norm_o.r, norm_o.g, norm_o.b, 1.0);
	//diffuse = 1.0;
	colorOut = vec4(vec3(diffuse), 1.0) * texture(myTexture, uv_o);
}