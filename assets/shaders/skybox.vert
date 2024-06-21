#version 450


layout(set = 0,binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
   vec3 cameraPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 TexCoords;

void main() 
{
    mat4 viewWithNoPos = ubo.view;
    viewWithNoPos[3][0] = 0.0;
    viewWithNoPos[3][1] = 0.0;
    viewWithNoPos[3][2] = 0.0;

    TexCoords = inPosition;
    vec4 pos = ubo.proj * viewWithNoPos * vec4(inPosition, 1.0);
    gl_Position = pos.xyww;
}