#version 450
#include "camera.glsl"


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// TO DO use inused float in mat4 to store RGB value
// x x x x
// x x x x
// x x x x
// Y Y Y x
// use Y
// extract Y Y Y 
// replace with 0 0 0

layout(location = 3) in vec4 InstanceMatrixColumus0;
layout(location = 4) in vec4 InstanceMatrixColumus1;
layout(location = 5) in vec4 InstanceMatrixColumus2;
layout(location = 6) in vec4 InstanceMatrixColumus3;


layout(location = 0) out vec3 outColor;


void main()
{
    mat4 instanceMatrix = mat4(
    InstanceMatrixColumus0,
    InstanceMatrixColumus1,
    InstanceMatrixColumus2,
    InstanceMatrixColumus3
    );

    vec3 color = vec3(instanceMatrix[0].w, instanceMatrix[1].w, instanceMatrix[2].w);

    instanceMatrix[0] = vec4(instanceMatrix[0].x, instanceMatrix[0].y, instanceMatrix[0].z, 0);
    instanceMatrix[1] = vec4(instanceMatrix[1].x, instanceMatrix[1].y, instanceMatrix[1].z, 0);
    instanceMatrix[2] = vec4(instanceMatrix[2].x, instanceMatrix[2].y, instanceMatrix[2].z, 0);
    
    
    instanceMatrix[3] = vec4(instanceMatrix[3].x - camera.cameraPos.x, 
    instanceMatrix[3].y - camera.cameraPos.y,
    instanceMatrix[3].z - camera.cameraPos.z,
    instanceMatrix[3].w);
    
    vec4 worldPos = instanceMatrix * vec4(inPosition, 1.0);
    gl_Position = camera.vp * worldPos;

    outColor = color;
}