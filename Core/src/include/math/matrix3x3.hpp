#pragma once

#include "vector3.hpp"

template <typename T>
class Matrix3x3
{
public:
    using DataType = T;
    using Vec = Vector3<DataType>;
    static constexpr size_t Size = 3;

    std::array<Vec,Size> data;

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
        data[0].x = x1;
        data[0].y = y1;
        data[0].z = z1;

        data[1].x = x2;
        data[1].y = y2;
        data[1].z = z2;

        data[2].x = x3;
        data[2].y = y3;
        data[2].z = z3;
        
    }
    constexpr Matrix3x3(DataType _value)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int k = 0; k < 3; k++)
            {
                data[i][k] = _value;
            }
        }
    }
    
    constexpr Matrix3x3(Vec _vec1,Vec _vec2)
    {
        data[0] = _vec1;
        data[1] = _vec2;
    }
    
    constexpr Matrix3x3(Vec _vec1)
    {
        data[0] = _vec1;
        data[1] = _vec1;
    }
    

    TOOLBOX_INLINE Vec operator[](size_t _size) const
    {
        return data[_size];
    }

    TOOLBOX_INLINE Vec& operator[](size_t _size)
    {
        return data[_size];
    }

    Matrix3x3 operator*(const Matrix3x3& other)
    {
        Matrix3x3 result = 0;
        
        for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                for (size_t k = 0; k < 3; k++)
                {
                    result.data[i][j] += data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }

    void operator*=(Matrix3x3 _m2)
    {
        *this = *this * _m2;
    }

    Vector3<T> Trace()
    {
        return {data[0][0],data[1][1],data[2][2]};
    }
    
    std::string ToString() const
    {
        return std::to_string(data[0].x) + " " + std::to_string(data[1].x) + " " + std::to_string(data[2].x)
         + '\n'
         + std::to_string(data[0].y) + " " + std::to_string(data[1].y) + " " + std::to_string(data[2].y)
         + '\n' +
          std::to_string(data[0].z) + " " + std::to_string(data[1].z) + " " + std::to_string(data[2].z)
         + '\n' ;
    }
};
