struct PSInput
{
    float4 Position : SV_POSITION;
    float3 ViewSpacePosition : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float2 TexCoord : TEXCOORD2;
    float3 Tangent : TEXCOORD3;
};

float4 Main(PSInput input) : SV_Target
{
   
	return float4(1, 1, 1, 1);
}