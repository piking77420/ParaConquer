#version 450

layout(location = 0) out vec4 outColor;

layout(set = ENVIRONEMENT_DESCRIPTOR_SET, binding = SKYBOX_BINDING) uniform samplerCube cubeMap;


layout(location = 0) in vec3 TexCoords;

const float gamma = 2.2;

void main()
{
    vec3 color = pow(texture(cubeMap, TexCoords).rgb, vec3(gamma)); // linear space to SRGB space

    outColor = vec4(color, 1);
}

