#pragma once
#include "vector2.hpp"


template <typename T>
class Matrix2x2
{
public:
    using DataType = T;
    using Vec = Vector2<DataType>;
    static constexpr size_t Size = 2;

    Matrix2x2 Identity()
    {
        Matrix2x2 result;

        for (int i = 0; i < data.size(); ++i)
        {
            result[i][i] = 1.f;
        }

        return result;
    }
    
    std::array<Vec,Size> data;

    constexpr Matrix2x2() = default;

    ~Matrix2x2() = default;

    constexpr Matrix2x2(DataType x1, DataType y1, DataType x2, DataType y2)
    {
        data[0].x = x1;
        data[0].y = y1;
        
        data[1].x = x2;
        data[1].y = y2;
    }
    constexpr Matrix2x2(DataType _value)
    {
        data[0].x = _value;
        data[0].y = _value;
        
        data[1].x = _value;
        data[1].y = _value;
    }
    
    constexpr Matrix2x2(Vec _vec1,Vec _vec2)
    {
        data[0] = _vec1;
        data[1] = _vec2;
    }
    
    constexpr Matrix2x2(Vec _vec1)
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

    Matrix2x2 operator*(Matrix2x2 other)
    {
        Matrix2x2 result = 0;
        
        for (size_t i = 0; i < 2; i++)
        {
            for (size_t j = 0; j < 2; j++)
            {
                for (size_t k = 0; k < 2; k++)
                {
                    result.data[i][j] += data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }

    void operator*=(Matrix2x2 _m2)
    {
        *this = *this * _m2;
    }

    Vector2<T> Trace()
    {
        return {data[0][0],data[1][1],data[2][2]};
    }

    std::string ToString() const
    {
        return  std::to_string(data[0].x) + " " + std::to_string(data[1].x) + " "
         + '\n'
         + std::to_string(data[0].y) + " " + std::to_string(data[1].y) + " "
         + '\n';
    }

};
