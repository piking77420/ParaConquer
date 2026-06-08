#ifndef FUNC_HEADER
#define FUNC_HEADER

float3 SRGBToLinear(float3 c)
{
    return lerp(c / 12.92,
                pow((c + 0.055) / 1.055, 2.4),
                step(0.04045, c));
}

#endif