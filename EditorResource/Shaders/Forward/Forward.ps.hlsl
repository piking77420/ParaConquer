struct PSInput
{
	float4 Position : SV_POSITION;
	float3 ViewSpacePosition : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float2 TextCoord : TEXCOORD2;
};

float4 Main(PSInput input) : SV_Target
{
   
	return float4(1, 1, 1, 1);
}