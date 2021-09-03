#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 norm;
layout (location = 3) in vec4 boneIDs;
layout (location = 4) in vec4 weights;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform mat4 u_bones[40];

out vec4 pos_o;
out vec3 pos_world_o;
out vec3 norm_o;
out vec2 uv_o;

void main()
{
	bool foundBone = false;
	pos_o = vec4(0.0, 0.0, 0.0, 0.0);
	for (int i = 0; i < 4; i++)
	{
		int index = int(boneIDs[i]);
		if (index != -1)
		{
			pos_o += (u_bones[index] * vec4(pos, 1.0) * weights[i]);
			foundBone = true;
		}
	}
	if (foundBone)
		pos_o = (u_projection * u_view * u_model * vec4(pos_o.xyz, 1.0));
	else
		pos_o = (u_projection * u_view * u_model * vec4(pos, 1.0));
		
	pos_world_o = (u_model * vec4(pos, 1.0)).xyz;
	norm_o = norm;
	uv_o = vec2(uv.x, 1.0 - uv.y);
	gl_Position = pos_o;
}