
#define GLOBAL_DESCRIPTOR 0
#define INSTANCE_DESCRIPTOR 1

layout(set = GLOBAL_DESCRIPTOR, binding = 0) uniform SceneBufferGPU
{
    mat4 view;
    mat4 proj;
    float time;
    float deltatime;
} sceneBuffer;

layout(push_constant) uniform ModelMatrix
{
    mat4 model;
} modelMatrix;
