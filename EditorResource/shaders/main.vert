#version 450
#include "camera.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragpos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;


layout(push_constant) uniform constants {
    mat4 model;
    mat4 normalInvMatrix;
} PushConstants;

void main() 
{
    vec4 worldPos = PushConstants.model * vec4(inPosition, 1.0);
    gl_Position = camera.vp * worldPos;
    
    fragpos = worldPos.xyz;
    fragNormal = normalize(mat3(PushConstants.normalInvMatrix) * inNormal);
    fragTexCoord = inTexCoord;
}
