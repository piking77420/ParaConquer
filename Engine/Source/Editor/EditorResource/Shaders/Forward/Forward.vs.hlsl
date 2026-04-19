

#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"


struct VsInput
{
    float4 Position : POSITION; // location 0
    float4 Normal : NORMAL; // location 1
    float4 Tangent : TEXCOORD0; // location 2
    float2 TexCoord : TEXCOORD1; // location 3
    float2 Pad : TEXCOORD2; // location 3
};

struct VsOutput
{
    float4 Position : SV_POSITION;
    float3 ViewSpacePosition : TEXCOORD0; // location 0
#if defined(LIT)
    float3 Normal : TEXCOORD1; // location 1
    float3 Tangent : TEXCOORD2; // location 2
#endif

#if defined(USE_UV)
    float2 TexCoord : TEXCOORD3; // location 3
#endif
};

struct PushConstant
{
    float4x4 modelView;
    float4x4 normalInvMatrixView;
};

[[vk::push_constant]]
PushConstant pushConstant;

VsOutput Main(VsInput input)
{
    VsOutput output;
    
    float3 PositionL = input.Position.xyz;
    float4 ViewPos = mul(float4(PositionL, 1.0), pushConstant.modelView); 
    output.ViewSpacePosition = ViewPos.xyz; // View position
    output.Position = mul(ViewPos, Projection);
#if defined(LIT)
    float3 NormalL = input.Normal.xyz;
    float3 TangentL = input.Tangent.xyz;
    output.Normal = normalize(mul(NormalL, (float3x3) pushConstant.normalInvMatrixView));
    output.Tangent = normalize(mul(TangentL, (float3x3) pushConstant.normalInvMatrixView));
#endif 

#if defined(USE_UV)
    output.TexCoord = input.TexCoord;
#endif

#if defined(USE_COLOR)

#if defined(DRAW_TRIANGLE)
     output.color = float4(frac(instanceID * 0.37), frac(instanceID * 0.61), frac(instanceID * 0.83), 1.0);
#endif

#endif 


    return output;
}
