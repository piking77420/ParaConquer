struct PSInput
{
    float4 Pos : SV_POSITION; 
    float3 Color : COLOR0; 
};

float4 Main(PSInput pSInput) : SV_Target
{
    return float4(pSInput.Color, 1.0); 
}