
#define CAMERA_BINDING b0
#define CAMERA_SPACE space0

#include "Camera.hlsl"




struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

static const float2 positions[4] =
{
    float2(-1.0, -1.0),
    float2(1.0, -1.0),
    float2(-1.0, 1.0),
    float2(1.0, 1.0)
};

static const float2 coords[4] =
{
    float2(0.0, 0.0),
    float2(1.0, 0.0),
    float2(0.0, 1.0),
    float2(1.0, 1.0)
};

struct PushConstant
{
    float4x4 model;
};

[[vk::push_constant]]
PushConstant pushConstants;


VS_OUT Main(uint vertexID : SV_VertexID)
{
    VS_OUT output;
    float4 worldPos = mul(pushConstants.model, float4(positions[vertexID], 0.0, 1.0));
    output.texCoord = coords[vertexID];
    output.position = mul(vp, worldPos);
    return output;
}