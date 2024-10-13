#define GLOBAL_DESCRIPTOR 0
#define INSTANCE_DESCRIPTOR 1

layout(set = GLOBAL_DESCRIPTOR, binding = 0) uniform SceneBuffer 
{
    mat4 view;
    mat4 proj;
} sceneBuffer;