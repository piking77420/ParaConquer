#ifndef LIGHT_HEADER
#define LIGHT_HEADER

struct DirLight
{
    float3 Direction;
    float pad0;
    float4 ColorIntensity;
};

#define POINT_LIGHT 0
#define SPOTH_LIGHT 1

struct Light
{
    float4 PositionType; // xyz = position / direction, w = type
    float4 ColorIntensity; // rgb = color, a = intensity
    float4 Params; // spot angles, radius, unused
};

StructuredBuffer<Light> Lights : register(LIGHT_BUFFER_BINDING, LIGHT_BUFFER_SPACE);

cbuffer LightsHeader : register(LIGHT_HEADER_BUFFER_BINDING, LIGHT_HEADER_BUFFER_SPACE)
{
    DirLight DirLightW;
    uint LightCount;
};

#endif // LIGHT_HEADER