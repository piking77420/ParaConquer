

#define CAMERA_BINDING b0
#define CAMERA_SPACE space0
#include "Camera.hlsl"


#define MATERIAL_SET space1
#include "Material.hlsl"

struct VsInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
    float3 Tangent : TEXCOORD0;
	float2 TexCoord : TEXCOORD0;
};

struct VsOutPut
{
    float4 Position : SV_POSITION;
    float3 ViewSpacePosition : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD1;
};

struct PushConstant
{
	float4x4 model;
	float4x4 normalInvMatrix;
};

[[vk::push_constant]]
PushConstant pushConstant;

VsOutPut Main(VsInput input)
{
    VsOutPut output;
    
    // position * model * view 
    float4 viewSpacePos = mul(view, mul(pushConstant.model, float4(input.Position, 1.0)));
    
    // Ready For rasterization
    output.Position = mul(proj, viewSpacePos);
    
     // Position in view space
    output.ViewSpacePosition = viewSpacePos.xyz;

    // Normal 
    float3x3 view3x3 = (float3x3) view;
    float3x3 normalInv3x3 = (float3x3) pushConstant.normalInvMatrix;
    output.Normal = normalize(mul(view3x3, mul(input.Normal, normalInv3x3)));
    output.Tangent = normalize(mul(view3x3, mul(normalInv3x3, input.Tangent)));
    
    // UV
    output.TexCoord = input.TexCoord;
    
    return output;
}
