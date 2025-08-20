
#define MAX_DIRLIGHT 1
#define MAX_POINTLIGHT 10
#define MAX_SPOTLIGHT 10

// Light data structures
struct DirectionalData
{
    float3 direction;
    float intensity;
    float3 color;
    float padding; // padding to 16-byte alignment
};

struct SpotLightData
{
    float3 position;
    float intensity;
    float3 direction;
    float cutoff;
    float3 color;
    float outCutOff;
    float maxRange;
    float padding; // optional, to maintain 16-byte alignment
};

struct PointLightData
{
    float3 position;
    float maxRange;
    float3 color;
    float intensity;
};

// -----------------------------------------------------------------------------
// Dynamic light data buffer
cbuffer DynamicLightData : register(LIGHT_BUFFER_BINDING, LIGHT_BUFFER_SPACE)
{
    DirectionalData dirLights[MAX_DIRLIGHT];
    SpotLightData spotLights[MAX_SPOTLIGHT];
    PointLightData pointLights[MAX_POINTLIGHT];

    int dirLightCount;
    int spotLightCount;
    int pointLightCount;
    int _pad; // padding to 16-byte alignment
};

// -----------------------------------------------------------------------------
// Square falloff attenuation
float GetSquareFalloffAttenuation(float3 posToLight, float lightInvRadius)
{
    float distanceSquare = dot(posToLight, posToLight);
    float factor = distanceSquare * lightInvRadius * lightInvRadius;
    float smoothFactor = max(1.0 - factor * factor, 0.0);
    return (smoothFactor * smoothFactor) / max(distanceSquare, 1e-4);
}
