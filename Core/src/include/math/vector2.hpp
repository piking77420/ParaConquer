#pragma once
#include <immintrin.h>
#include <stacktrace>

#include "toolbox_headerfile.hpp"

template <typename T>
class Vector2
{
public:
    using DataType = T;
    
    T x = static_cast<T>(0);
    T y = static_cast<T>(0);

    constexpr Vector2() = default;

    constexpr Vector2(DataType valueX, DataType valueY) : x(valueX), y(valueY) {}

    constexpr Vector2(DataType valueX) : x(valueX), y(valueX) {}

    ~Vector2() = default;

    TOOLBOX_INLINE constexpr DataType* GetPtr()
    {
        return &x;
    }

    TOOLBOX_INLINE constexpr const DataType* GetPtr() const
    {
        return &x;
    }
    constexpr static TOOLBOX_INLINE DataType Dot(const Vector2 _v1, const Vector2 _v2)
    {
        return _v1.x * _v2.x + _v1.y * _v2.y;
    }

    TOOLBOX_INLINE DataType MagnitudeSquare() const
    {
        Vector2 v = *this;
        return Dot(v, v);
    }

    TOOLBOX_INLINE DataType Magnitude() const
    {
        return std::sqrt(MagnitudeSquare());
    }

    TOOLBOX_INLINE Vector2 Normalize() const
    {
        const DataType mag = Magnitude();
        
        if (IsEqualTo<DataType>(mag,static_cast<T>(1)))
            return *this;
        
        const DataType InvMagnitude = static_cast<T>(1) / mag;
        
        return { x * InvMagnitude, y * InvMagnitude};    
    }

    
    
    constexpr static TOOLBOX_INLINE DataType Cross(const Vector2 _v1, const Vector2 _v2)
    {
        return _v1.x * _v2.y -  _v1.y * _v2.x;
    }

    TOOLBOX_INLINE Vector2 operator-() const
    {
        return {-x, -y};
    }

    TOOLBOX_INLINE Vector2 operator+(const Vector2 _other) const
    {
        const __m128 a = _mm_set_ps(0.0f, 0.0f, y, x);
        const __m128 b = _mm_set_ps(0.0f, 0.0f, _other.y, _other.x);

        const __m128 result = _mm_add_ps(a, b);
            
        float res[4];
        _mm_store_ps(res, result);
            
        return {res[0] , res[1]};
    }

    
    TOOLBOX_INLINE Vector2 operator-(const Vector2 _other) const
    {
        const __m128 a = _mm_set_ps(0.0f, 0.0f, y, x);
        const __m128 b = _mm_set_ps(0.0f, 0.0f, _other.y, _other.x);

        const __m128 result = _mm_sub_ps(a, b);
            
        float res[4];
        _mm_store_ps(res, result);
            
        return {res[0] , res[1]};
    }

    TOOLBOX_INLINE Vector2 operator*(const Vector2 _other) const
    {
        const __m128 a = _mm_set_ps(0.0f, 0.0f, y, x);
        const __m128 b = _mm_set_ps(0.0f, 0.0f, _other.y, _other.x);

        const __m128 result = _mm_mul_ps(a, b);
            
        float res[4];
        _mm_store_ps(res, result);
            
        return {res[0] , res[1]};
    }

    TOOLBOX_INLINE Vector2 operator/(const Vector2 _other) const
    {
        const __m128 a = _mm_set_ps(0.0f, 0.0f, y, x);
        const __m128 b = _mm_set_ps(0.0f, 0.0f, _other.y, _other.x);

        const __m128 result = _mm_div_ps(a, b);
            
        float res[4];
        _mm_store_ps(res, result);
            
        return {res[0] , res[1]};
    }

    TOOLBOX_INLINE void operator+=(const Vector2 _other)
    {
        const __m128 a = _mm_set_ps(0.0f, 0.0f, y, x);
        const __m128 b = _mm_set_ps(0.0f, 0.0f, _other.y, _other.x);

        const __m128 result = _mm_add_ps(a, b);
            
        float res[4];
        _mm_store_ps(res, result);
            
        x = res[0];
        y = res[1];
    }

    TOOLBOX_INLINE void operator-=(const Vector2 _other)
    {
        const __m128 a = _mm_set_ps(0.0f, 0.0f, y, x);
        const __m128 b = _mm_set_ps(0.0f, 0.0f, _other.y, _other.x);

        const __m128 result = _mm_sub_ps(a, b);
            
        float res[4];
        _mm_store_ps(res, result);
            
        x = res[0];
        y = res[1];
    }

    TOOLBOX_INLINE void operator*=(const Vector2 _other)
    {
        const __m128 a = _mm_set_ps(0.0f, 0.0f, y, x);
        const __m128 b = _mm_set_ps(0.0f, 0.0f, _other.y, _other.x);

        const __m128 result = _mm_mul_ps(a, b);
            
        float res[4];
        _mm_store_ps(res, result);
            
        x = res[0];
        y = res[1];
    }

    TOOLBOX_INLINE void operator/=(const Vector2 _other)
    {
        const __m128 a = _mm_set_ps(0.0f, 0.0f, y, x);
        const __m128 b = _mm_set_ps(0.0f, 0.0f, _other.y, _other.x);

        const __m128 result = _mm_div_ps(a, b);
            
        float res[4];
        _mm_store_ps(res, result);
            
        x = res[0];
        y = res[1];
    }

    TOOLBOX_INLINE Vector2 operator+(DataType _value) const
    {
        return {x + _value , y + _value};
    }
    
    TOOLBOX_INLINE Vector2 operator-(DataType _value) const
    {
        return {x - _value , y - _value};
    }
    
    TOOLBOX_INLINE Vector2 operator*(DataType _value) const
    {
        return {x * _value , y * _value};
    }
    
    TOOLBOX_INLINE Vector2 operator/(DataType _value) const
    {
        return {x / _value , y / _value};
    }

    
    TOOLBOX_INLINE DataType& operator[](size_t index)
    {
        return *static_cast<T*>(&x + index);
    }

    TOOLBOX_INLINE const DataType& operator[](size_t index) const
    {
        return *static_cast<const T*>(&x + index);
    }

    TOOLBOX_INLINE bool operator==(const Vector2& _other) const
    {
        return x == _other.x && y == _other.y;
    }
    
    TOOLBOX_INLINE bool operator!=(const Vector2& _other)
    {
        return x != _other.x || y != _other.y;
    }

    std::string ToString() const
    {
       return  "X : " + std::to_string(x) + ", Y : " + std::to_string(y) + '\n';
    }
private:
    
};
