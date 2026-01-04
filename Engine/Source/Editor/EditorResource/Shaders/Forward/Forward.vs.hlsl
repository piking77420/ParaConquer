

#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"

struct VsInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TANGENT;
};

struct VsOutput
{
    float4 Position : SV_POSITION;
    float3 ViewSpacePosition : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float2 TexCoord : TEXCOORD2;
    float3 Tangent : TEXCOORD3;
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

    // Clip-space position
    output.Position = mul(ViewPos, Projection);

    // Pass to fragment shader
    output.ViewSpacePosition = ViewPos;
    output.Normal = normalize(mul(input.Normal, (float3x3) pushConstant.normalInvMatrixView));

    output.Tangent = input.Tangent;
    output.TexCoord = input.TexCoord;

    return output;
}
