#version 450

#include "include/buffers.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextCoords;

layout(location = 0) out vec4 fragColor;

layout( push_constant ) uniform constants
{
	vec4 color;
	mat4 model;
} wireframeData;


void main()
{
    vec4 position = sceneBuffer.proj * sceneBuffer.view * wireframeData.model * vec4(inPosition, 1.0); 
    gl_Position = position;
	fragColor = wireframeData.color;
}