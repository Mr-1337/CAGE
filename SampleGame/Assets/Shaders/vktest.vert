#version 450

#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;
layout(location = 2) out vec4 colorOut;

layout(push_constant) uniform mouse
{
    vec4 data;
} PushConstants;

void main() {
    colorOut = color;

    vec3 newp = vec3(pos.x, -pos.y, -pos.z);
    float angle = PushConstants.data.z;
    newp.x *= sin(angle);
    newp.x += newp.z * cos(angle);
    //newp.z *= cos(angle) + pos.x * sin(angle);
    //newp.x /= newp.z;
    //newp.y /= newp.z;
    newp.z += 0.3;
    vec3 offset = vec3((PushConstants.data.x - 800) / 800, (PushConstants.data.y - 450) / 450,0);
    newp += offset;

    gl_Position = vec4(newp, 1.0);
}