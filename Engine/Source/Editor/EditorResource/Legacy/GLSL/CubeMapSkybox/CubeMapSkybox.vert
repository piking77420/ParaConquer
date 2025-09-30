#version 450

#include "camera.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;


layout(location = 0) out vec3 TexCoords;

void main() 
{
    TexCoords = inPosition;
    vec4 pos = camera.vp * vec4(inPosition, 1.0);
    gl_Position = pos.xyww;
}
