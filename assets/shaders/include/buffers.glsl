
// This descriptor must Containt Unifrom Camera at Binding 1 and final texture to viewPort
#define SCENE_DESCRIPTOR 0
#define INSTANCE_DESCRIPTOR 1

layout(set = SCENE_DESCRIPTOR, binding = 0) uniform SceneBufferGPU
{
    mat4 view;
    mat4 proj;
    float time;
    float deltatime;
} sceneBuffer;


	