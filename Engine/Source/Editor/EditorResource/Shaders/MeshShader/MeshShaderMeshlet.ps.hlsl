struct PSInput
{
    float4 Position : SV_POSITION;
#if defined(USE_COLOR)
    float3 Color : COLOR;
#endif
};


float4 Main(PSInput pSInput) : SV_Target
{
    float4 Lo = float4(0,0,0, 1);

#if defined(USE_COLOR)
    Lo.xyz = pSInput.Color;
#endif
    
    return Lo;
}