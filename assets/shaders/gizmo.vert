#version 450

layout(push_constant) uniform pushConstant 
{
    layout(offset = 0) mat4 trs;
    layout(offset = 64) vec3 color;
}ps;


layout(set = 0,binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

void main() 
{
    vec4 fragPosition = ps.trs * vec4(inPosition, 1.0);

    gl_Position = ubo.proj * ubo.view * fragPosition;
}