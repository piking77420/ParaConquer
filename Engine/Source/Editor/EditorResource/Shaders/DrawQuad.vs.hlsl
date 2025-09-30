struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

VSOutput Main(uint vertexID : SV_VertexID)
{
    VSOutput output = (VSOutput)0;

    float2 positions[4] = {
        float2(-1.0, -1.0), // bottom-left
        float2( 1.0, -1.0), // bottom-right
        float2(-1.0,  1.0), // top-left
        float2( 1.0,  1.0)  // top-right
    };

    float2 coords[4] = {
        float2(0.0, 0.0), // bottom-left
        float2(1.0, 0.0), // bottom-right
        float2(0.0, 1.0), // top-left
        float2(1.0, 1.0)  // top-right
    };

    output.position = float4(positions[vertexID], 0.0, 1.0);
    output.texCoord = coords[vertexID];

    return output;
}