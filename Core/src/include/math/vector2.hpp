#pragma once
#include <stacktrace>

#include "toolbox_headerfile.hpp"

template <typename T>
class Vector2
{
public:
    using DataType = T;
    
    T x = 0.f;
    T y = 0.f;

    constexpr Vector2() = default;

    constexpr Vector2(DataType valueX, DataType valueY) : x(valueX), y(valueY) {}

    constexpr Vector2(DataType valueX) : x(valueX), y(valueX) {}

    ~Vector2() = default;

    constexpr DataType* GetPtr()
    {
        return &x;
    }

    constexpr const DataType* GetPtr() const
    {
        return &x;
    }

    DataType Magnitude() const 
    {
        return std::sqrt(x * x + y * y);
    }

    TOOLBOX_INLINE Vector2 Normalize() const
    {
        const DataType mag = Magnitude();
        
        if (IsEqualTo<DataType>(mag,static_cast<T>(1)))
            return this;
        
        const DataType InvMagnitude = static_cast<T>(1) / mag;
        
        return { x * InvMagnitude, y * InvMagnitude};    
    }

    constexpr static TOOLBOX_INLINE DataType Dot(const Vector2 _v1, const Vector2 _v2)
    {
        return _v1.x * _v2.x + _v1.y * _v2.y;         
    }
    
    constexpr static TOOLBOX_INLINE DataType Cross(const Vector2 _v1, const Vector2 _v2)
    {
        return _v1.x * _v2.y -  _v1.y * _v2.x;
    }

    Vector2 operator-()
    {
        return {-x, -y};
    }

    Vector2 operator+(const Vector2 _other)
    {
        return {x + _other.x , y + _other.y};
    }

    Vector2 operator-(const Vector2 other)
    {
        return {x - other.x , y - other.y};
    }

    Vector2 operator*(const Vector2 _other)
    {
        return {x * _other.x , y * _other.y};
    }

    Vector2 operator/(const Vector2 _other)
    {
        return {x / _other.x , y / _other.y};
    }

    Vector2 operator+(DataType _value)
    {
        return {x + _value , y + _value};
    }
    
    Vector2 operator-(DataType _value)
    {
        return {x - _value , y - _value};
    }
    
    Vector2 operator*(DataType _value)
    {
        return {x * _value , y * _value};
    }
    
    Vector2 operator/(DataType _value)
    {
        return {x / _value , y / _value};
    }

    
    TOOLBOX_INLINE DataType& operator[](size_t _index)
    {
        return *(GetPtr() + _index);
    }

    TOOLBOX_INLINE DataType operator[](size_t _index) const
    {
        return *(GetPtr() + _index);
    }

    TOOLBOX_INLINE bool operator==(const Vector2& _other)
    {
        return IsEqualTo(x,_other.x) && IsEqualTo(y,_other.y);
    }
    
    TOOLBOX_INLINE bool operator!=(const Vector2& _other)
    {
        return !(this == _other);
    }

    std::string ToString() const
    {
       return  "X : " + std::to_string(x) + ", Y : " << std::to_string(y) + '\n';
    }
private:
    
};
