#version 450


#include "camera.glsl"


layout(set = SCENE_DESCRIPTOR_SET, binding = 3) uniform ViewFrustumBuffer {
    mat4 camToWorldMatrix;
    vec3 viewParams;
} frustum;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 uv;



void main() 
{
    vec3 viewPointLocal = vec3(uv - 0.5, 1) * frustum.viewParams;
    vec3 viewPoint = normalize(vec3(frustum.camToWorldMatrix * vec4(viewPointLocal, 1.0))) ;
    
    outColor = vec4(viewPoint, 1.0); 
}