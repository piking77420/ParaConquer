#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out float Height;
layout(location = 1) out vec2 TexCoords;

layout(set = 0,binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
 vec3 cameraPos;
} ubo;



void main()
{
    
    mat4 translate = mat4(1);
    vec3 cameraPos = ubo.cameraPos;
    cameraPos.y -= 0.2f;
    translate[3][0] = cameraPos.x;
    translate[3][1] = cameraPos.y;
    translate[3][2] = cameraPos.z;

    vec4 pos = ubo.proj * ubo.view * translate * vec4(inPosition, 1.0);
    TexCoords = inTexCoord;

    Height = inPosition.y;
    // Set output variables
    gl_Position = pos.xyww;
}