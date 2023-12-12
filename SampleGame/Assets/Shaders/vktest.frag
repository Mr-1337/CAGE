#version 450

layout(location = 0) out vec4 outColor;
layout(location = 2) in vec4 inColor;

layout (origin_upper_left) in vec4 gl_FragCoord;

layout(push_constant) uniform mouse
{
    vec4 data;
} PushConstants;

void main() {
    vec4 highlight = vec4(vec3(1.0, 0.0, 0.4) * 30.0 * (1.0 / distance(PushConstants.data, gl_FragCoord)), 0.0);
    outColor = inColor + highlight;

}