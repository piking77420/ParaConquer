
layout(set = 0, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
    mat4 vp;
    mat4 vpIn;
    mat4 viewInv;
    mat4 projInv;
    float time;
    float deltatime;
} ubo;
