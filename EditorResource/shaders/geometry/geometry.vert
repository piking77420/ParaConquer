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
    vec4 worldPos = camera.vp * PushConstants.model * vec4(inPosition, 1.0);
    vec3 normal = normalize(mat3(PushConstants.normalInvMatrix) * inNormal);
    outNormal = normal;
    
    outWorldPosition = worldPos;
    gl_Position = worldPos;
    outTexCoord = inTexCoord;
}
