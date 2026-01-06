struct PSInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TEXCOORD1;
};

float4 Main(PSInput input) : SV_Target
{
   
	return float4(1, 1, 1, 1);
}