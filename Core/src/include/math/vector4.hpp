#pragma once
#include "toolbox_headerfile.hpp"

template <typename T>
class Vector4
{
public:
using DataType = T;
    
    T x = 0.f;
    T y = 0.f;
    T z = 0.f;
    T w = 0.f;

    constexpr Vector4() = default;

    constexpr Vector4(DataType valueX, DataType valueY, DataType valueZ, DataType valueW) : x(valueX), y(valueY) ,z(valueZ), w(valueW) {}

    constexpr Vector4(DataType valueX) : x(valueX), y(valueX), z(valueX), w(valueX) {}

    ~Vector4() = default;

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
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    Vector4<DataType> Normalize() const
    {
        const DataType mag = Magnitude();
        
        if (IsEqualTo<DataType>(mag,static_cast<T>(1)))
            return this;
        
        const DataType InvMagnitude = static_cast<T>(1) / mag;
        
        return { x * InvMagnitude, y * InvMagnitude, z * InvMagnitude, w * InvMagnitude };    
    }
    
    constexpr static float Dot(const Vector4& _v1, const Vector4& _v2)
    {
        return _v1.x * _v2.x + _v1.y * _v2.y + _v1.z * _v2.z + _v1.w * _v2.w;         
    }
    
    constexpr static TOOLBOX_INLINE DataType DistanceSquare(const Vector4& _a, const Vector4& _b)
    {
        const DataType dx = _a.x - _b.x;
        const DataType dy = _a.y - _b.y;
        const DataType dz = _a.z - _b.z;
        const DataType dw = _a.w - _b.w;
        const DataType result = (dx * dx + dy * dy + dz * dz);

        return result;
    }

    [[nodiscard]]
    static TOOLBOX_INLINE DataType Distance(const Vector4& _a, const Vector4& _b)
    {
        return std::sqrt(DistanceSquare(_a, _b));
    }

    constexpr Vector4 operator-()
    {
        return {-x, -y, -z, -w};
    }

    constexpr Vector4 operator+(const Vector4& _other)
    {
        return {x + _other.x , y + _other.y, z + _other.z, w + _other.w};
    }

    constexpr Vector4 operator-(const Vector4& _other)
    {
        return {x - _other.x , y - _other.y, z - _other.z, w - _other.w};
    }

    constexpr Vector4 operator*(const Vector4& _other)
    {
        return {x * _other.x , y * _other.y, z * _other.z, w * _other.w};
    }

    constexpr Vector4 operator/(const Vector4& _other)
    {
        return {x / _other.x , y / _other.y, z / _other.z,  w / _other.w};
    }

    constexpr TOOLBOX_INLINE void operator+= (const Vector4& _other) noexcept
    {
        x += _other.x;
        y += _other.y;
        z += _other.z;
        w += _other.w;
    }

    constexpr TOOLBOX_INLINE void operator-= (const Vector4& _other) noexcept
    {
        x -= _other.x;
        y -= _other.y;
        z -= _other.z;
        w -= _other.w;
    }

    constexpr TOOLBOX_INLINE void operator*= (const Vector4& _other) noexcept
    {
        x *= _other.x;
        y *= _other.y;
        z *= _other.z;
        w *= _other.w;
    }

    constexpr TOOLBOX_INLINE void operator/= (const Vector4& _other) noexcept
    {
        x /= _other.x;
        y /= _other.y;
        z /= _other.z;
        w /= _other.w;
    }
    
    constexpr Vector4 operator+(DataType value)
    {
        return {x + value , y + value, z + value, w + value};
    }

    constexpr Vector4 operator-(DataType value)
    {
        return {x - value , y - value, z - value, w - value};
    }

    constexpr Vector4 operator*(DataType value)
    {
        return {x * value , y * value, z * value, w * value};
    }

    constexpr Vector4 operator/(DataType value)
    {
        return {x / value , y / value, z / value, w / value};
    }
    
    TOOLBOX_INLINE DataType& operator[](size_t index)
    {
        return *static_cast<T*>(&x + index);
    }

    TOOLBOX_INLINE const DataType& operator[](size_t index) const
    {
        return *static_cast<const T*>(&x + index);
    }
    
    TOOLBOX_INLINE bool operator==(const Vector4& _other)
    {
        return IsEqualTo(x,_other.x) && IsEqualTo(y,_other.y) && IsEqualTo(z,_other.z) && IsEqualTo(w,_other.w);
    }
    
    TOOLBOX_INLINE bool operator!=(const Vector4& _other)
    {
        return !(this == _other);
    }
    
    constexpr Vector4 Zero() { return Vector4(0); };

    constexpr Vector4 UnitX() { return { 1, 0 ,0 }; };
    
    constexpr Vector4 UnitY() { return { 0, 1 ,0 }; };
    
    constexpr Vector4 UnitZ() { return { 0, 0 ,1 }; };
    
    constexpr Vector4 UnitW() { return { 0, 0 ,1 , 1 }; };

    [[nodiscard]]
    std::string ToString() const
    {
        return "X : " + std::to_string(x) + ", Y : " + std::to_string(y) + ", Z : " + std::to_string(z) + ", W : " <<  std::to_string(w) << '\n';
    }
};
