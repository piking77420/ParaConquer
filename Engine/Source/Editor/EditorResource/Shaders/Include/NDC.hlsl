#ifndef NDC_HEADER
#define NDC_HEADER

#if defined(VULKAN)

static const float3 NdcCorner[8] =
{
    // Near plane, z = 0
    float3(-1.0,  1.0, 0.0), // 0 Near top left
    float3( 1.0,  1.0, 0.0), // 1 Near top right
    float3( 1.0, -1.0, 0.0), // 2 Near bottom right
    float3(-1.0, -1.0, 0.0), // 3 Near bottom left

    // Far plane, z = 1
    float3(-1.0,  1.0, 1.0), // 4 Far top left
    float3( 1.0,  1.0, 1.0), // 5 Far top right
    float3( 1.0, -1.0, 1.0), // 6 Far bottom right
    float3(-1.0, -1.0, 1.0)  // 7 Far bottom left
};

static const uint FrustumIndices[24] =
{
    // Near plane
    0, 1,
    1, 2,
    2, 3,
    3, 0,

    // Far plane
    4, 5,
    5, 6,
    6, 7,
    7, 4,

    // Connecting edges
    0, 4,
    1, 5,
    2, 6,
    3, 7
};

#endif // defined(VULKAN)

#endif // NDC_HEADER