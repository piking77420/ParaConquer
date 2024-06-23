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

        for (int i = 0; i < m_Data.size(); ++i)
        {
            result[i][i] = 1.f;
        }

        return result;
    }
    

    constexpr Matrix2x2() = default;

    ~Matrix2x2() = default;

    constexpr Matrix2x2(DataType x1, DataType y1, DataType x2, DataType y2)
    {
        m_Data[0].x = x1;
        m_Data[0].y = y1;
        
        m_Data[1].x = x2;
        m_Data[1].y = y2;
    }
    constexpr Matrix2x2(DataType _value)
    {
        m_Data[0].x = _value;
        m_Data[0].y = _value;
        
        m_Data[1].x = _value;
        m_Data[1].y = _value;
    }
    
    constexpr Matrix2x2(Vec _vec1,Vec _vec2)
    {
        m_Data[0] = _vec1;
        m_Data[1] = _vec2;
    }
    
    constexpr Matrix2x2(Vec _vec1)
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

    Matrix2x2 operator*(const Matrix2x2& _other)
    {
        Matrix2x2 result = 0;

        for (size_t rows = 0; rows < Size; rows++)
        {
            for (size_t coloms = 0; coloms < Size; coloms++)
            {
                for (size_t dot = 0; dot < Size; dot++)
                {
                    result[coloms][rows] += m_Data[dot][rows] * _other[coloms][dot];
                }
            }
        }
        return result;
    }

    void operator*=(Matrix2x2 _m2)
    {
        *this = *this * _m2;
    }

    bool operator==(const Matrix2x2& _other) const
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

    Vector2<T> Trace()
    {
        return {m_Data[0][0],m_Data[1][1],m_Data[2][2]};
    }

    std::string ToString() const
    {
        return  std::to_string(m_Data[0].x) + " " + std::to_string(m_Data[1].x) + " "
         + '\n'
         + std::to_string(m_Data[0].y) + " " + std::to_string(m_Data[1].y) + " "
         + '\n';
    }
private:
        std::array<Vec, Size> m_Data;

};
