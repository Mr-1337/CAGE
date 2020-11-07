#version 460 core

in vec4 pos_o;
in vec3 pos_world_o;
in vec2 uv_o;
in vec3 norm_o;
out vec4 colorOut;

uniform sampler2D u_texture;

void main()
{
	//float diffuse = max(dot(norm_o, normalize(-pos_world_o + vec3(200.0, 20.0, 200.0))) / (0.05 * length(-pos_world_o + vec3(200.0, 20.0, 200.0))), 0.001);
	float diffuse = max(dot(norm_o, normalize(-pos_world_o + vec3(200.0, 20.0, 200.0))), 0.1);
	//colorOut = vec4(norm_o.r, norm_o.g, norm_o.b, 1.0);

	colorOut = vec4(diffuse, diffuse, diffuse, 1.0) * texture(u_texture, uv_o);
}