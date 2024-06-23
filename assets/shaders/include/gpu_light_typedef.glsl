

#define MAX_POINTLIGHT_COUNT  10
#define MAX_SPOTLIGHT_COUNT   10
#define MAX_DIRLIGHT_COUNT    1



struct GpuDirLight
{
    vec3 direction;
    vec3 color;
    float intensity;
};

struct GpuPointLight
{
    vec3 position;
    vec3 color;
    float intensity;
};

struct GpuSpotLight
{
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
};
