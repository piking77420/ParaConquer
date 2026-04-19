struct PSInput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};


float4 Main(PSInput pSInput) : SV_Target
{
    return float4(pSInput.Color, 1.0);
}