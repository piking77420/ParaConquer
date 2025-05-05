
#define SCENE_DESCRIPTOR 0
#define LIGHT_SCENE_DATA_BINDING 1

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


layout(set = SCENE_DESCRIPTOR, binding = LIGHT_SCENE_DATA_BINDING) uniform LightSceneData
{
	int dirlightCount;
	int pointLightCount;
	int spothLightCount;

	GpuDirLight gpuDirLight;
	GpuPointLight gpuPointLights[];
	GpuSpotLight gpuSpotLights[];	    


} lightSceneData;

