

#define CAMERA_BINDING b0
#define CAMERA_SPACE space0
#include "Camera.hlsl"

struct VsInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

struct VsOutput
{
    float4 Position : SV_POSITION;
    float3 ViewSpacePosition : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float2 TextCoord : TEXCOORD2;
};

struct PushConstant
{
    float4x4 model;
    float4x4 normalInvMatrix;
};

[[vk::push_constant]]
PushConstant pushConstant;

VsOutput Main(VsInput input)
{
    VsOutput output;

    // World position
    float4 worldPos = mul(float4(input.Position, 1.0), pushConstant.model);

    // Clip-space position
    output.Position = mul(worldPos,vp);

    // Pass to fragment shader
    output.ViewSpacePosition = worldPos.xyz;
    output.Normal = normalize(mul(input.Normal, (float3x3) pushConstant.normalInvMatrix));
    output.TextCoord = input.TexCoord;

    return output;
}
