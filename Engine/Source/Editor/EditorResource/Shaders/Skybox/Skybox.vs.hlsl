struct VSInput
{
    [[vk::location(0)]] float3 Position : POSITION0;
};

#define CAMERA_BINDING b0
#define CAMERA_SPACE space0

#include "Camera.hlsl"

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float3 dir : TEXCOORD0; // direction pour le cubemap
};

VSOutput Main(VSInput input, uint VertexID : SV_VertexID)
{
    VSOutput output;
    float4 clipPos = mul(vp, float4(input.Position, 1.0f));
    output.Pos = clipPos.xyww;
    output.dir = input.Position;
    return output;
}