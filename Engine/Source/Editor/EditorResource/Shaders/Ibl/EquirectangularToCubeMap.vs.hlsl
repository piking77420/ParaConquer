struct PushConstant
{
    float4x4 ViewProjection;
};

[[vk::push_constant]]
PushConstant pushConstant;

struct VsInPut
{
    float3 Position : POSITION;
};

struct VsOutPut
{
    float4 Position : SV_POSITION;
}

VsOutPut Main(VsInPut input) : SV_TARGET
{
    VsOutPut output;
    output.Position = pushConstant.ViewProjection * float4(input.Position, 1.0);

    return output;
}