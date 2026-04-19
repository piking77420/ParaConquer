#ifndef HASH_COLOR
#define HASH_COLOR

uint HashCombine(uint a, uint b)
{
    return a * 1664525u + b * 1013904223u;
}

float hash(uint x)
{
    x ^= x >> 17;
    x *= 0xed5ad4bb;
    x ^= x >> 11;
    x *= 0xac4c1b51;
    x ^= x >> 15;
    x *= 0x31848bab;
    x ^= x >> 14;
    return frac(x / 65536.0);
}

float3 hash3(uint id)
{
    return float3(
        hash(id),
        hash(id + 1),
        hash(id + 2)
    );
}

float3 hsv2rgb(float3 c)
{
    float4 K = float4(1., 2./3., 1./3., 3.);
    float3 p = abs(frac(c.xxx + K.xyz) * 6. - K.www);
    return c.z * lerp(K.xxx, saturate(p - K.xxx), c.y);
}

float3 HashColor(uint id)
{
    id ^= 2747636419u;
    id *= 2654435769u;
    id ^= id >> 16;
    id *= 2654435769u;
    id ^= id >> 16;

    float r = float((id >>  0) & 0xFF) / 255.0;
    float g = float((id >>  8) & 0xFF) / 255.0;
    float b = float((id >> 16) & 0xFF) / 255.0;

    return float3(r, g, b);
}



#endif // HashColor