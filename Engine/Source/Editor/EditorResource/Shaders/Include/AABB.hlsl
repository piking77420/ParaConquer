#ifndef AABB_HEADER_HLSL
#define AABB_HEADER_HLSL

struct AABB
{
    float4 Min;
    float4 Max;
}

float4 Center(AABB Aabb)
{
    return (Aabb.Min + Aabb.Max) * 0.5f;  
}

float4 Extend(AABB Aabb)
{
    return (Aabb.Max - Aabb.Min) * 0.5f;  
}

AABB TransformAABB(AABB Aabb, float4x4 Matrix)
{
    float4 GlobalCenter = mul(Matrix, float4(Center(Aabb).xyz, 1.0));
    float4 Extend = float4(Extend(Aabb).xyz, 0.0);
    float3x3 AbsMatrix = abs((float3x3)Matrix);
    float3 NewExtent = mul(AbsMatrix, Extend).xyz;

    AABB newAabb;
    newAabb.min = GlobalCenter - NewExtent;
    newAabb.max = GlobalCenter + NewExtent;

    return newAabb;
}


#endif