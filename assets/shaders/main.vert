#version 450


#include "include/buffers.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextCoords;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;


void main()
{
    vec4 position = sceneBuffer.proj * sceneBuffer.view * modelMatrix.model * vec4(inPosition, 1.0); 
    vec3 normal = inNormal;

    gl_Position = position;
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix.model)));
    fragColor = normalMatrix * inNormal;
    fragTexCoord = inTextCoords;
}