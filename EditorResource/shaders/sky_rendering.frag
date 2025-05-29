#version 450
#include "camera.glsl"


layout(location = 0) out vec4 outColor;


layout(set = SCENE_DESCRIPTOR_SET, binding = VIEWFRUSTUM_BINDING) uniform ViewFrustumBuffer {
    vec3 topLeft;
    vec3 topRight;
    vec3 bottomLeft;
    vec3 bottomRight;
} frustum;




void main() 
{
    outColor = vec4(1.0,1.0,1.0,1.0);
}