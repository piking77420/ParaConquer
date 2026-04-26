struct PSInput
{
    float4 Pos : SV_POSITION; 
    float4 Color : COLOR0; 
};

float4 Main(PSInput pSInput) : SV_Target
{
    return pSInput.Color; 
}