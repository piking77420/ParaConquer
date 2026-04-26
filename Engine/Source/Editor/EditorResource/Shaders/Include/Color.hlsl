#ifndef COLOR_INC
#define COLOR_INC

    float4 FromPackedRGB(uint32_t value)
    {
        float r = ((value >> 24) & 0xFF);
        float g = ((value >> 16) & 0xFF);
        float b = ((value >> 8)  & 0xFF);
        float a = ((value >> 0)  & 0xFF);

        return float4(r, g, b, a );
    }

#endif // COLOR_INC