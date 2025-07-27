
struct DirectionalData
{
    vec3 direction;
    float intensity;
    vec3 color;
    float padding;
};

struct SpotLightData
{
    vec3 position;
    float intensity;
    vec3 direction;
    float cutoff;
    vec3 color;
    float outCutOff;
    float maxRange;
};

struct PointLightData
{
    vec3 position;
    float maxRange;
    vec3 color;
    float intensity;
};


layout(set = SCENE_DESCRIPTOR_SET, binding = LIGHTDATA_BINDING) uniform DynamicLightData
{
    DirectionalData dirlights[MAX_DIRLIGHT];
    SpotLightData spothLights[MAX_SPOTLIGHT];
    PointLightData pointLights[MAX_POINTLIGHT];
    int dirLightCount;
    int spothLightCount;
    int pointLightCount;
} lightSceneData;

