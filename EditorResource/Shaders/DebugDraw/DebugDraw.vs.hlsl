#define CAMERA_BINDING b0
#define CAMERA_SPACE space0

#include "Camera.hlsl"

struct VSInput
{
    [[vk::location(0)]] float3 Position : POSITION0;

    // Per-instance transform matrix columns
    [[vk::location(1)]] float4 InstanceMatrixColums0 : POSITION1;
    [[vk::location(2)]] float4 InstanceMatrixColums1 : POSITION2;
    [[vk::location(3)]] float4 InstanceMatrixColums2 : POSITION3;
    [[vk::location(4)]] float4 InstanceMatrixColums3 : POSITION4;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    [[vk::location(0)]] float3 Color : COLOR0;
};

VSOutput Main(VSInput vSInput, uint VertexID : SV_VertexID)
{
    VSOutput vsOutPut = (VSOutput)0;
    
    float4x4 instanceMatrix = float4x4(
        vSInput.InstanceMatrixColums0,
        vSInput.InstanceMatrixColums1,
        vSInput.InstanceMatrixColums2,
        vSInput.InstanceMatrixColums3
    );

    float3 color = float3(
        instanceMatrix[0].w,
        instanceMatrix[1].w,
        instanceMatrix[2].w
    );

    instanceMatrix[0].w = 0.0;
    instanceMatrix[1].w = 0.0;
    instanceMatrix[2].w = 0.0;


    instanceMatrix[3].xyz -= cameraPos.xyz;

    float4 worldPos = mul(float4(vSInput.Position, 1.0), instanceMatrix);
    vsOutPut.Pos = mul(worldPos, vp);

    vsOutPut.Color = color;
    return vsOutPut;
}
