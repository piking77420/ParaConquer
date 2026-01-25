

#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"


struct VsInput
{
    float3 Position : POSITION; // location 0
    float3 Normal : NORMAL; // location 1
    float3 Tangent : TEXCOORD0; // location 2
    float2 TexCoord : TEXCOORD1; // location 3
};

struct VsOutput
{
    float4 Position : SV_POSITION;
    float3 ViewSpacePosition : TEXCOORD0; // location 0
    float3 Normal : TEXCOORD1; // location 1
    float3 Tangent : TEXCOORD2; // location 2
    float2 TexCoord : TEXCOORD3; // location 3
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

    // World position
    float4 ViewPos = mul(float4(input.Position, 1.0), pushConstant.modelView);

    output.Position = mul(ViewPos, Projection);
    output.ViewSpacePosition = ViewPos.xyz;
    output.Normal = normalize(mul(input.Normal, (float3x3) pushConstant.normalInvMatrixView));
    output.Tangent = normalize(mul(input.Tangent, (float3x3) pushConstant.normalInvMatrixView));
    output.TexCoord = input.TexCoord;

    return output;
}
