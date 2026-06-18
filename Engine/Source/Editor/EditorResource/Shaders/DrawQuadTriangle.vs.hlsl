struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

VSOutput Main(uint vertexID : SV_VertexID)
{
    VSOutput output = (VSOutput) 0;

    output.texCoord = float2((vertexID << 1) & 2, vertexID & 2);
    output.position = float4(output.texCoord * 2.0 - 1.0, 0.0, 1.0);

    return output;
}
