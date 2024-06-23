#pragma once

#include "vector3.hpp"

template <typename T>
class Matrix3x3
{
public:
    using DataType = T;
    using Vec = Vector3<DataType>;
    static constexpr size_t Size = 3;


    constexpr Matrix3x3() = default;

    ~Matrix3x3() = default;

    static Matrix3x3 Identity()
    {
        Matrix3x3 result;

        for (int i = 0; i < 3; ++i)
        {
            result[i][i] = static_cast<DataType>(1);
        }

        return result;
    }

    constexpr Matrix3x3(DataType x1, DataType y1, DataType z1,
        DataType x2, DataType y2, DataType z2,
        DataType x3, DataType y3, DataType z3)
    {
        m_Data[0].x = x1;
        m_Data[0].y = y1;
        m_Data[0].z = z1;

        m_Data[1].x = x2;
        m_Data[1].y = y2;
        m_Data[1].z = z2;

        m_Data[2].x = x3;
        m_Data[2].y = y3;
        m_Data[2].z = z3;
    }

    constexpr Matrix3x3(const Vec& _vec1 , const Vec& _vec2 , const Vec& _vec3)
    {
        m_Data[0] = _vec1;
        m_Data[1] = _vec2;
        m_Data[2] = _vec3;
    }
    constexpr Matrix3x3(DataType _value)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int k = 0; k < 3; k++)
            {
                m_Data[i][k] = _value;
            }
        }
    }
    
    constexpr Matrix3x3(Vec _vec1,Vec _vec2)
    {
        m_Data[0] = _vec1;
        m_Data[1] = _vec2;
    }
    
    constexpr Matrix3x3(Vec _vec1)
    {
        m_Data[0] = _vec1;
        m_Data[1] = _vec1;
    }
    

    TOOLBOX_INLINE Vec operator[](size_t _size) const
    {
        return m_Data[_size];
    }

    TOOLBOX_INLINE Vec& operator[](size_t _size)
    {
        return m_Data[_size];
    }

    Matrix3x3 operator*(const Matrix3x3& _other)
    {
        Matrix3x3 result = {};
        

        for (size_t rows = 0; rows < Size; rows++)
        {
            for (size_t coloms = 0; coloms < Size; coloms++)
            {
                for (size_t dot = 0; dot < Size; dot++)
                {
                    result.m_Data[coloms][rows] += m_Data[dot][rows] * _other[coloms][dot];
                }
            }
        }
        return result;
    }

    void operator*=(Matrix3x3 _m2)
    {
        *this = *this * _m2;
    }

    bool operator==(const Matrix3x3& _other) const
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

    Vector3<T> Trace()
    {
        return {m_Data[0][0],m_Data[1][1],m_Data[2][2]};
    }
    
    std::string ToString() const
    {
        return std::to_string(m_Data[0].x) + " " + std::to_string(m_Data[1].x) + " " + std::to_string(m_Data[2].x)
         + '\n'
         + std::to_string(m_Data[0].y) + " " + std::to_string(m_Data[1].y) + " " + std::to_string(m_Data[2].y)
         + '\n' +
          std::to_string(m_Data[0].z) + " " + std::to_string(m_Data[1].z) + " " + std::to_string(m_Data[2].z)
         + '\n' ;
    }

    Matrix3x3 Transpose() const
    {
        Matrix3x3 result = 0;

        for (size_t i = 0; i < Size; i++)
        {
            for (size_t j = 0; j < Size; j++)
            {
                result[i][j] = m_Data[j][i];
            }
        }
        return result;
    }
 private:
        std::array<Vec, Size> m_Data;

    
};
