

#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"

#include "HashColor.hlsl"

#define RENDER_INSTANCE_BUFFER_BINDING t1
#define RENDER_INSTANCE_BUFFER_SPACE space0
#include "InstanceBuffer.hlsl"

struct VsInput
{
    float4 Position : POSITION; // location 0
    float4 Normal : NORMAL; // location 1
    float4 Tangent : TEXCOORD0; // location 2
    float2 TexCoord : TEXCOORD1; // location 3
    float2 Pad : TEXCOORD2; // location 3r
    uint InstanceID : SV_InstanceID;
    uint VertexID   : SV_VertexID;
};

struct VsOutput
{
    float4 Position : SV_POSITION;
#if defined(LIT) || defined(VIEWPOS)

#if defined(VIEWPOS)
    float3 WorldSpacePosition : TEXCOORD0;
#elif defined(LIT)
    float3 WorldSpacePosition : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 Tangent : TEXCOORD2;
    #endif
#endif

#if defined(USE_UV)
    float2 TexCoord : TEXCOORD3; // location 3
#endif

#if defined(USE_COLOR)
    nointerpolation float3 Color : COLOR0;
#endif
};

struct PushConstant
{
    uint RenderInstanceID;
};

[[vk::push_constant]]
PushConstant pushConstant;

VsOutput Main(VsInput input)
{
    VsOutput output;
    
    RenderInstance renderInstance = RenderInstances[pushConstant.RenderInstanceID];
    float4 WorldPos =  mul(renderInstance.ModelView, float4(input.Position.xyz, 1.0));
    float4 Pos = mul(View, WorldPos); // model view is just a model in fact

    // Positions
#if defined(LIT) || defined(VIEWPOS)
    output.WorldSpacePosition = WorldPos.xyz; // World position
#endif
    output.Position = mul(ClipSpaceCorrection, mul(Projection, Pos));

    // Lit dependencies
#if defined(LIT)
    float3 NormalL = input.Normal.xyz;
    float3 TangentL = input.Tangent.xyz;
    float3x3 Model3 = (float3x3)renderInstance.ModelView;

    float3x3 NormalInverseView = (float3x3)renderInstance.NormalInverseMatrixView; // no view component
    output.Normal = normalize(mul(NormalInverseView, NormalL));
    float3 TangentV = normalize(mul(Model3, TangentL));
    output.Tangent = float4(TangentV, input.Tangent.w);
#endif 

    // Need uvs
#if defined(USE_UV)
    output.TexCoord = input.TexCoord;
#endif

    // Colors Passes
#if defined(USE_COLOR)

#if defined(DRAW_TRIANGLE)
    uint combined = input.InstanceID * 73856093u ^ input.VertexID * 19349663u;
    output.Color = hash3(combined);
#endif

#endif 

    return output;
}
