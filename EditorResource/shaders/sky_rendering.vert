#version 450


layout(set = SCENE_DESCRIPTOR_SET, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
    mat4 vp;
    mat4 vpIn;
    mat4 viewInv;
    mat4 projInv;
    float time;
    float deltatime;
} camera;
layout(set = SCENE_DESCRIPTOR_SET, binding = 3) uniform ViewFrustumBuffer {
    vec3 topLeft;
    vec3 topRight;
    vec3 bottomLeft;
    vec3 bottomRight;
} frustum;





vec2 positions[4] = vec2[](
    vec2(-1.0, -1.0), // 0: bottom-left
    vec2( 1.0, -1.0), // 1: bottom-right
    vec2(-1.0,  1.0), // 2: top-left
    vec2( 1.0,  1.0)  // 3: top-right
);
    
void main() 
{
    vec4 pos = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    pos = pos.xyww;
    gl_Position = pos; 
}
