#version 450

layout(location = 0) out vec4 outColor;

layout(set = ENVIRONEMENT_DESCRIPTOR_SET, binding = SKYBOX_BINDING) uniform samplerCube cubeMap;


layout(location = 0) in vec3 TexCoords;

void main()
{
    outColor = texture(cubeMap, TexCoords);
}

