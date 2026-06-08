#ifndef CUBE_HLSL
#define CUBE_HLSL

static const float3 CubeVertices[8] =
{
    float3(-0.5, -0.5, -0.5), // 0
    float3( 0.5, -0.5, -0.5), // 1
    float3( 0.5,  0.5, -0.5), // 2
    float3(-0.5,  0.5, -0.5), // 3

    float3(-0.5, -0.5,  0.5), // 4
    float3( 0.5, -0.5,  0.5), // 5
    float3( 0.5,  0.5,  0.5), // 6
    float3(-0.5,  0.5,  0.5)  // 7
};


static const float2 CubeTextCoord[24] =
{
    // Front face +Z
    float2(0.0, 1.0),
    float2(1.0, 1.0),
    float2(1.0, 0.0),
    float2(0.0, 0.0),

    // Back face -Z
    float2(0.0, 1.0),
    float2(1.0, 1.0),
    float2(1.0, 0.0),
    float2(0.0, 0.0),

    // Left face -X
    float2(0.0, 1.0),
    float2(1.0, 1.0),
    float2(1.0, 0.0),
    float2(0.0, 0.0),

    // Right face +X
    float2(0.0, 1.0),
    float2(1.0, 1.0),
    float2(1.0, 0.0),
    float2(0.0, 0.0),

    // Top face +Y
    float2(0.0, 1.0),
    float2(1.0, 1.0),
    float2(1.0, 0.0),
    float2(0.0, 0.0),

    // Bottom face -Y
    float2(0.0, 1.0),
    float2(1.0, 1.0),
    float2(1.0, 0.0),
    float2(0.0, 0.0),
};

static const uint CubeIndices[36] =
{
    // Front
    4, 5, 6,
    6, 7, 4,

    // Back
    1, 0, 3,
    3, 2, 1,

    // Left
    0, 4, 7,
    7, 3, 0,

    // Right
    5, 1, 2,
    2, 6, 5,

    // Top
    3, 7, 6,
    6, 2, 3,

    // Bottom
    0, 1, 5,
    5, 4, 0
};

#endif // CUBE_HLSL