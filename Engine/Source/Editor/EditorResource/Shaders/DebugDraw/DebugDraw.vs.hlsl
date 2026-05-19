#include "Color.hlsl"
#include "NDC.hlsl"

#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"

#if defined(INSTANCED)
StructuredBuffer<float4x4> RenderInstances : register(t1, space0);
#else 
struct DebugDrawCall
{
	float4x4 ModelView;
};
[[vk::push_constant]]
DebugDrawCall DrawCall;
#endif // defined(INSTANCED)

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

    // Get Matrix
    float4x4 Matrix =
#if defined(INSTANCED) 
    RenderInstances[InstanceID];
#else
    DrawCall.ModelView;
#endif // defined(INSTANCED) 

#if defined(INSTANCED)
    float3 VertexPos = vSInput.Position;
     // Get Color
    float4 color = FromPackedRGB(asuint(Matrix[3][3]));
    color.w = 1.0f;
    Matrix[3][3] = 1.0f;


    float4 ViewPos = mul(Matrix, float4(VertexPos, 1.0));
    vsOutPut.Pos = mul(Projection, ViewPos);

#elif defined(FRUSTUM)
    float4 FrustumWorldPos = mul(Matrix, float4(NdcCorner[FrustumIndices[VertexID]], 1.0));
    FrustumWorldPos.xyz /= FrustumWorldPos.w;
    FrustumWorldPos.w = 1.0;

    vsOutPut.Pos = mul(ViewProjection, FrustumWorldPos);
    float4 color = float4(1,1,1,1);
#endif

    vsOutPut.Color = color;
    return vsOutPut;
}
