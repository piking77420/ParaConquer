#version 450


layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;


layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);

    vec4 vec = ubo.model * vec4(inNormal.x,inNormal.y,inNormal.z,1);
    fragColor = vec3(vec.x,vec.y,vec.z);
}