#include "Cube.hlsl"

struct PushConstant
{
    float4x4 ViewProjectionCorr;
    float Metadata;
};

[[vk::push_constant]]
PushConstant pushConstant;


struct VsOutPut
{
    float4 Position : SV_POSITION;
#if defined(USE_UV)
    float3 TexCoord : TEXCOORD0;
#endif
};

VsOutPut Main(uint vertexIndex : SV_VertexID)
{
    VsOutPut output;
    uint index = CubeIndices[vertexIndex];
    float3 position = CubeVertices[index];
    output.Position = mul(pushConstant.ViewProjectionCorr, float4(position, 1.0));
#if defined (SKYBOX_FORCE_FAR_DEPTH)
output.Position.z = output.Position.w;
#endif
#if defined(USE_UV)
    output.TexCoord = normalize(position);
#endif

    return output;
}