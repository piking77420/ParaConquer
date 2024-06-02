#pragma once

#include "vector4.hpp"

template <typename T>
class Matrix4x4
{
public:
    using DataType = T;
    using Vec = Vector4<DataType>;
    static constexpr size_t Size = 4;
    
    std::array<Vec,Size> data;

    constexpr Matrix4x4() = default;

    ~Matrix4x4() = default;

    static Matrix4x4 Identity()
    {
        Matrix4x4 result;
        
        for (int i = 0; i < Size; i++)
        {
            result[i][i] = 1.f;
        }

        return result;
    }
    
    constexpr Matrix4x4(const DataType x1, const DataType y1, const DataType z1, const DataType w1,
        const DataType x2, const DataType y2, const DataType z2, const DataType w2,
        const DataType x3, const DataType y3, const DataType z3, const DataType w3,
         const DataType x4, const DataType y4, const DataType z4, const DataType w4)
    {
        data[0].x = x1;
        data[0].y = y1;
        data[0].z = z1;
        data[0].w = w1;

        data[1].x = x2;
        data[1].y = y2;
        data[1].z = z2;
        data[1].w = w2;

        data[2].x = x3;
        data[2].y = y3;
        data[2].z = z3;
        data[2].w = w3;

        data[3].x = x4;
        data[3].y = y4;
        data[3].z = z4;
        data[3].w = w4;

    }
    constexpr Matrix4x4(DataType _value)
    {
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t k = 0; k < 4; k++)
            {
                data[i][k] = _value;
            }
        }
    }
    
    constexpr Matrix4x4(const Vec _vec1, const Vec _vec2, const Vec _vec3, const Vec _vec4)
    {
        data[0] = _vec1;
        data[1] = _vec2;
        data[2] = _vec3;
        data[3] = _vec4;

    }
    
    constexpr Matrix4x4(const Vec _vec1)
    {
        data[0] = _vec1;
        data[1] = _vec1;
        data[2] = _vec1;
        data[3] = _vec1;
    }

    TOOLBOX_INLINE const Vec& operator[](size_t _size) const
    {
        return data[_size];
    }

    TOOLBOX_INLINE Vec& operator[](size_t _size)
    {
        return data[_size];
    }

    Matrix4x4 operator*(const Matrix4x4& _other)
    {
        Matrix4x4 result = 0;
        
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                for (size_t k = 0; k < 4; k++)
                {
                    result.data[i][j] += data[i][k] * _other.data[k][j];
                }
            }
        }
        return result;
    }

    Vec operator*(const Vec& _vector4)
    {
        Vec result = 0;
        
        for (size_t i = 0; i < Size; ++i)
        {
            for (size_t j = 0; j < Size; ++j) {
                result.data[i] += data[i][j] * _vector4.data[j];
            }
        }
        
        return result;
    }

    void operator*=(Matrix4x4 _m2)
    {
        *this = *this * _m2;
    }

    Vector4<T> Trace()
    {
        return {data[0][0],data[1][1],data[2][2]};
    }

    std::string ToString() const
    {
        return std::to_string(data[0].x) + " " + std::to_string(data[1].x) + " " + std::to_string(data[2].x) + " " + std::to_string(data[3].x)
         + '\n'
         + std::to_string(data[0].y) + " " + std::to_string(data[1].y) + " " + std::to_string(data[2].y) + " " + std::to_string(data[3].y)
         + '\n'
         + std::to_string(data[0].z) + " " + std::to_string(data[1].z) + " " + std::to_string(data[2].z) + " " + std::to_string(data[3].z)
         + '\n'
         + std::to_string(data[0].w) + " " + std::to_string(data[1].w) + " " + std::to_string(data[2].w) + " " + std::to_string(data[3].w)
         + '\n';
    }
};

    