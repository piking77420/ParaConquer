#pragma once

#include "vector4.hpp"

template <typename T>
class Matrix4x4
{
public:
    using DataType = T;
    using Vec = Vector4<DataType>;
    static constexpr size_t Size = 4;
    

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
    
    constexpr Matrix4x4(DataType x1, DataType y1, DataType z1, DataType w1,
        DataType x2, DataType y2, DataType z2, DataType w2,
        DataType x3, DataType y3, DataType z3, DataType w3,
         DataType x4, DataType y4, DataType z4, DataType w4)
    {
        m_Data[0].x = x1;
        m_Data[0].y = y1;
        m_Data[0].z = z1;
        m_Data[0].w = w1;

        m_Data[1].x = x2;
        m_Data[1].y = y2;
        m_Data[1].z = z2;
        m_Data[1].w = w2;

        m_Data[2].x = x3;
        m_Data[2].y = y3;
        m_Data[2].z = z3;
        m_Data[2].w = w3;

        m_Data[3].x = x4;
        m_Data[3].y = y4;
        m_Data[3].z = z4;
        m_Data[3].w = w4;

    }
    constexpr Matrix4x4(DataType _value)
    {
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t k = 0; k < 4; k++)
            {
                m_Data[i][k] = _value;
            }
        }
    }
    
    constexpr Matrix4x4(const Vec _vec1, const Vec _vec2, const Vec _vec3, const Vec _vec4)
    {
        m_Data[0] = _vec1;
        m_Data[1] = _vec2;
        m_Data[2] = _vec3;
        m_Data[3] = _vec4;

    }
    
    constexpr Matrix4x4(const Vec _vec1)
    {
        m_Data[0] = _vec1;
        m_Data[1] = _vec1;
        m_Data[2] = _vec1;
        m_Data[3] = _vec1;
    }

    constexpr TOOLBOX_INLINE const DataType* GetPtr() const noexcept
    {
        return &m_Data[0][0];
    }

    TOOLBOX_INLINE DataType* GetPtr() noexcept
    {
        return &m_Data[0][0];
    }


    TOOLBOX_INLINE const Vec& operator[](size_t _size) const
    {
        return m_Data[_size];
    }

    TOOLBOX_INLINE Vec& operator[](size_t _size)
    {
        return m_Data[_size];
    }

    Matrix4x4 operator*(const Matrix4x4& _other) const
    {
        Matrix4x4 result = {};
        
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                for (size_t k = 0; k < 4; k++)
                {
                    result.m_Data[i][j] += m_Data[i][k] * _other.m_Data[k][j];
                }
            }
        }
        return result;
    }

    Matrix4x4 Transpose() const
    {
        Matrix4x4 result = 0;

        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                result[i][j] = m_Data[j][i];
            }
        }
        return result;
    }
    
    Vec operator*(const Vec& _vector4)
    {
        Vec result = 0;
        
        for (size_t i = 0; i < Size; i++)
        {
            for (size_t j = 0; j < Size; j++)
            {
                result[i] += m_Data[i][j] * _vector4[j];
            }
        }
        
        return result;
    }

    bool operator==(const Matrix4x4& _other) const
    {
        
        for (size_t i = 0; i < Size; ++i)
        {
            for (size_t j = 0; j < Size; ++j) 
            {
               if (m_Data[i][j] != _other[i][j])
               {
                   return false;
               }
            }
        }

        return true;
    }

    void operator*=(Matrix4x4 _m2)
    {
        *this = *this * _m2;
    }

    Vector4<T> Trace()
    {
        return {m_Data[0][0], m_Data[1][1], m_Data[2][2], m_Data[3][3]};
    }

    std::string ToString() const
    {
        return std::to_string(m_Data[0].x) + " " + std::to_string(m_Data[1].x) + " " + std::to_string(m_Data[2].x) + " " + std::to_string(m_Data[3].x)
         + '\n'
         + std::to_string(m_Data[0].y) + " " + std::to_string(m_Data[1].y) + " " + std::to_string(m_Data[2].y) + " " + std::to_string(m_Data[3].y)
         + '\n'
         + std::to_string(m_Data[0].z) + " " + std::to_string(m_Data[1].z) + " " + std::to_string(m_Data[2].z) + " " + std::to_string(m_Data[3].z)
         + '\n'
         + std::to_string(m_Data[0].w) + " " + std::to_string(m_Data[1].w) + " " + std::to_string(m_Data[2].w) + " " + std::to_string(m_Data[3].w)
         + '\n';
    }
private:
        std::array<Vec, Size> m_Data;

};

    

template <typename T>
TOOLBOX_INLINE Matrix4x4<T> Multiply(const Matrix4x4<T>& matrix, const T _scalar)
{
    Matrix4x4<T> result;
    Matrix4x4<T> m = matrix.Transpose();

    const __m512 a = _mm512_set_ps(m[3][3], m[3][2], m[3][1], m[3][0],
        m[2][3], m[2][2], m[2][1], m[2][0],
        m[1][3], m[1][2], m[1][1], m[1][0],
        m[0][3], m[0][2], m[0][1], m[0][0]);

    const __m512 b = _mm512_set_ps(_scalar, _scalar, _scalar, _scalar,
        _scalar, _scalar, _scalar, _scalar,
        _scalar, _scalar, _scalar, _scalar,
        _scalar, _scalar, _scalar, _scalar);

    const __m512 result__m512 = _mm512_mul_ps(a, b);
    _mm512_store_ps(result.GetPtr(), result__m512);

    return result;
}