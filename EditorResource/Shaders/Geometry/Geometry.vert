#version 450
#include "camera.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;



layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec4 outWorldPosition;
layout(location = 3) out vec2 outTexCoord;

layout(push_constant) uniform constants {
    mat4 model;
    mat4 normalInvMatrix;
} PushConstants;

void main()
{
    // to do optimise this
    // math accurate but some mat can be done cpu side

    vec4 viewPos = camera.view * PushConstants.model * vec4(inPosition, 1.0);
    
    // normal 
    outNormal = normalize( mat3(camera.view) * mat3(PushConstants.normalInvMatrix) * inNormal);
    // viewPos
    outWorldPosition = viewPos;
    // fragpos
    gl_Position = camera.proj * viewPos;
    // uv
    outTexCoord = inTexCoord;
}
