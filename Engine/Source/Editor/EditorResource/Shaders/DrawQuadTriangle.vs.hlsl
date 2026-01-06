struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

VSOutput Main(uint vertexID : SV_VertexID)
{
    VSOutput output = (VSOutput) 0;

    // Fullscreen triangle positions (clip space)
    float2 positions[3] =
    {
        float2(-1.0, -1.0), // bottom-left
        float2(-1.0, 3.0), // top-left (overshoot)
        float2(3.0, -1.0) // bottom-right (overshoot)
    };

    // Corresponding texture coordinates
    float2 coords[3] =
    {
        float2(0.0, 0.0),
        float2(0.0, 2.0),
        float2(2.0, 0.0)
    };

    output.position = float4(positions[vertexID], 0.0, 1.0);
    output.texCoord = coords[vertexID];

    return output;
}
