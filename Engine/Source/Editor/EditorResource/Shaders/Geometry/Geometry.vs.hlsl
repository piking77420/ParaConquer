

#define CAMERA_BINDING b0
#define CAMERA_SPACE space0
#include "Camera.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
};

struct VS_OUT
{
	float3 Normal : NORMAL;
    float4 ViewSpacePosition : TEXCOORD0;
	float2 TexCoord : TEXCOORD1;
	float4 Position : SV_POSITION;
};


struct PushConstant
{
	float4x4 model;
	float4x4 normalInvMatrix;
};

[[vk::push_constant]]
PushConstant pushConstant;

VS_OUT Main(VS_IN input)
{
    VS_OUT output;

    // position * model * view 
    float4 viewSpacePos = mul(mul(float4(input.Position, 1.0), pushConstant.model), view);

    // Normal 
    float3x3 view3x3 = (float3x3) view;
    float3x3 normalInv3x3 = (float3x3) pushConstant.normalInvMatrix;
    output.Normal = normalize(mul(mul(input.Normal, normalInv3x3), view3x3));

    // position in view space
    output.ViewSpacePosition = viewSpacePos;

    // Screen space
    output.Position = mul(viewSpacePos, proj);

    // UV
    output.TexCoord = input.TexCoord;

    return output;
}
