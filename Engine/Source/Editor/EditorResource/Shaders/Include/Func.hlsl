#ifndef FUNC_HEADER
#define FUNC_HEADER

float3 SRGBToLinear(float3 c)
{
    return lerp(c / 12.92,
                pow((c + 0.055) / 1.055, 2.4),
                step(0.04045, c));
}

float3 LinearToSRGB(float3 x)
{
    x = max(x, 0.0);

    return select(
        x * 12.92,
        1.055 * pow(x, 1.0 / 2.4) - 0.055,
        x > 0.0031308
    );
}

float3 ApplyGammaCorrection(float3 Rgb, float gamma)
{
   return pow(Rgb, float3(1.0/gamma, 1.0/gamma, 1.0/gamma));
}

#endif