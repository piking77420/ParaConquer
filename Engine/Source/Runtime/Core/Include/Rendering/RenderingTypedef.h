#pragma once

#include <array>

#include "LowRenderer/RhiTypedef.h"

#define GPU_ALIGN alignas(16)

namespace PC_CORE::Rendering
{

    enum class RenderMode : uint8_t
    {
        TriangleBased,
        ClusterBased,
        PathTracing
    };
    REFLECT(RenderMode);

namespace Gpu
{
    struct mat4
    {
        std::array<float, 16> data;
    };

    struct mat3
    {
        std::array<float, 9> data;
    };

    struct vec4
    {
        std::array<float, 4> data;
    };

    struct vec3
    {
        std::array<float, 3> data;
    };

    struct vec2
    {
        std::array<float, 2> data;
    };

    static inline void StreamDoubleToFloat(vec2* _Dst, Tbx::Vector2d* _Src)
    {
        _Dst->data[0] = static_cast<float>(_Src->x);
        _Dst->data[1] = static_cast<float>(_Src->y);
    }

    static inline void StreamDoubleToFloat(vec3* _Dst, Tbx::Vector3d* _Src)
    {
        _Dst->data[0] = static_cast<float>(_Src->x);
        _Dst->data[1] = static_cast<float>(_Src->y);
        _Dst->data[2] = static_cast<float>(_Src->z);

    }

    static inline void StreamDoubleToFloat(vec4* _Dst, Tbx::Vector4d* _Src)
    {
        _Dst->data[0] = static_cast<float>(_Src->x);
        _Dst->data[1] = static_cast<float>(_Src->y);
        _Dst->data[2] = static_cast<float>(_Src->z);
        _Dst->data[3] = static_cast<float>(_Src->w);
    }

    static inline void StreamDoubleToFloat(mat3* _Dst, const Tbx::Matrix3x3d* _Src)
    {
        for (size_t i = 0; i < 9; i++)
        {
            _Dst->data[i] = static_cast<float>(_Src->data[i]);
        }
    }

    static inline void StreamDoubleToFloat(mat4* _Dst, const Tbx::Matrix4x4d* _Src)
    {
        for (size_t i = 0; i < 16; i++)
        {
            _Dst->data[i] = static_cast<float>(_Src->data[i]);
        }
    }

    static constexpr size_t MAX_LIGHT = 1024;

    struct GPU_ALIGN DirLight
    {
        vec3 Direction;
        float Pad0;
        vec4 ColorIntensity; // rgb = color, a = intensity
    };

    enum LightType : int
    {
        Point,
        Spoth,
    };

    // amy pack color in one float
    struct GPU_ALIGN Light
    {
        vec4 PositionType;   // xyz = position / direction, w = type
        vec4 ColorIntensity; // rgb = color, a = intensity
        vec4 Params;
    };

    struct GPU_ALIGN LightHeader {
        DirLight DirLight;
        uint32_t LightCount;
    };

}


}
