
#define CAMERA_BINDING b0
#define CAMERA_SPACE space0

#include "Camera.hlsl"

struct VSInput
{
    // Per instance
    [[vk::location(0)]] float4 Point1 : POSITION0;
    [[vk::location(1)]] float4 Point2 : POSITION1; // w = distance
    [[vk::location(2)]] float4 Color : COLOR0;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    [[vk::location(0)]] float3 Color : COLOR0;
};



VSOutput Main(VSInput vSInput, uint VertexID : SV_VertexID)
{
    VSOutput vSOutput = (VSOutput)0;

    float4 currentPoint = VertexID == 0 ? vSInput.Point1 : vSInput.Point2;
    float4 vertexCameraSpace = float4(currentPoint.x - cameraPos.x, currentPoint.y - cameraPos.y, currentPoint.z - cameraPos.z, 1);
    
    vSOutput.Pos = mul(vp, vertexCameraSpace);
    vSOutput.Color = vSInput.Color.rgb;
    
    return vSOutput;
}
