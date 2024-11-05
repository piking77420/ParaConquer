
// This descriptor must Containt Unifrom Camera at Binding 1 and final texture to viewPort
#define VIEWPORT_DESCRIPTOR 0
#define INSTANCE_DESCRIPTOR 1

layout(set = VIEWPORT_DESCRIPTOR, binding = 0) uniform SceneBufferGPU
{
    mat4 view;
    mat4 proj;
    float time;
    float deltatime;
} sceneBuffer;

layout(set = VIEWPORT_DESCRIPTOR, binding = 1) uniform sampler2D finalImageSampler;


layout(push_constant) uniform ModelMatrix
{
    mat4 model;
} modelMatrix;
