
#include "Color.hlsl"

#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"


StructuredBuffer<float4x4> RenderInstances : register(t1, space0);

struct VSInput
{
    [[vk::location(0)]] float3 Position : POSITION0;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

VSOutput Main(VSInput vSInput, 
    uint VertexID : SV_VertexID,
    uint InstanceID : SV_InstanceID)
{
    VSOutput vsOutPut = (VSOutput)0;
    float4x4 Matrix = RenderInstances[InstanceID];
    float4 color = FromPackedRGB(asuint(Matrix[3][3]));
    Matrix[3][3] = 1.0f;

    float4 worldPos = mul(float4(vSInput.Position, 1.0), Matrix);
    vsOutPut.Pos = mul(worldPos, Projection);

    vsOutPut.Color = color;
    return vsOutPut;
}
