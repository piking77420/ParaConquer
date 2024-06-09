#version 450
#define MAX_ENTITIES 10000

struct MeshesMatrix
{
    mat4 model;
    mat3 NormalInvert;
}; 

layout(std430, set = 0, binding = 1) buffer MeshesMatrixsBuffer 
{
    MeshesMatrix meshesMatrix[MAX_ENTITIES]; // Corrected the name to meshesMatrix
};

layout(push_constant) uniform pushConstant 
{
    layout(offset = 0) int meshesMatrixIndex;
}ps;

layout(set = 0,binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * meshesMatrix[ps.meshesMatrixIndex].model * vec4(inPosition, 1.0);
    fragColor = inNormal;
    fragTexCoord = inTexCoord;
}