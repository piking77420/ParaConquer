#pragma once
#include "toolbox_headerfile.hpp"

template <typename T>
class Vector3
{
public:
    using DataType = T;

    T x = 0.f;
    T y = 0.f;
    T z = 0.f;

    constexpr Vector3() = default;

    constexpr Vector3(DataType valueX, DataType valueY, DataType valueZ) : x(valueX), y(valueY), z(valueZ)
    {
    }

    constexpr Vector3(DataType valueX) : x(valueX), y(valueX), z(valueX)
    {
    }

    ~Vector3() = default;

    constexpr DataType* GetPtr()
    {
        return &x;
    }

    TOOLBOX_INLINE constexpr const DataType* GetPtr() const
    {
        return &x;
    }


    DataType MagnitudeSquare() const
    {
        return x * x + y * y + z * z;
    }
    
    DataType Magnitude() const
    {
        return std::sqrt(MagnitudeSquare());
    }

    Vector3 Normalize() const
    {
        const DataType mag = Magnitude();

        if (IsEqualTo<DataType>(mag, static_cast<T>(1)))
            return *this;

        const DataType InvMagnitude = static_cast<T>(1) / mag;

        return {x * InvMagnitude, y * InvMagnitude, z * InvMagnitude};
    }


    constexpr static DataType Dot(const Vector3& _v1, const Vector3& _v2)
    {
        return _v1.x * _v2.x + _v1.y * _v2.y + _v1.z * _v2.z;
    }

    constexpr static Vector3 Cross(const Vector3& _v1, const Vector3& _v2)
    {
        return Vector3(_v1.y * _v2.z - _v1.z * _v2.y,
                       _v1.z * _v2.x - _v1.x * _v2.z,
                       _v1.x * _v2.y - _v1.y * _v2.x);
    }

    constexpr static TOOLBOX_INLINE DataType DistanceSquare(const Vector3& _a, const Vector3& _b)
    {
        const DataType dx = _a.x - _b.x;
        const DataType dy = _a.y - _b.y;
        const DataType dz = _a.z - _b.z;
        const DataType result = (dx * dx + dy * dy + dz * dz);

        return result;
    }

    [[nodiscard]]
    static TOOLBOX_INLINE DataType Distance(const Vector3& _a, const Vector3& _b)
    {
        return std::sqrt(DistanceSquare(_a, _b));
    }

    // In radian
    static inline Vector3 FromSphericalCoord(DataType _radius, DataType _pitch, DataType _head)
    {
        DataType x = _radius * std::cos(_pitch) * std::sinf(_head);
        DataType y = _radius * -std::sinf(_pitch);
        DataType z = _radius * std::cos(_pitch) * std::cosf(_head);
        return {x, y, z};
    }


    Vector3 operator-()
    {
        return {-x, -y, -z};
    }

    Vector3 operator+(const Vector3& other) const
    {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vector3 operator-(const Vector3& other) const
    {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vector3 operator*(const Vector3& other) const
    {
        return {x * other.x, y * other.y, z * other.z};
    }

    Vector3 operator/(const Vector3& other) const
    {
        return {x / other.x, y / other.y, z / other.z};
    }

    constexpr TOOLBOX_INLINE void operator+=(const Vector3& _Row1) noexcept
    {
        x += _Row1.x;
        y += _Row1.y;
        z += _Row1.z;
    }

    constexpr TOOLBOX_INLINE void operator-=(const Vector3& _Row1) noexcept
    {
        x -= _Row1.x;
        y -= _Row1.y;
        z -= _Row1.z;
    }

    constexpr TOOLBOX_INLINE void operator*=(const Vector3& _Row1) noexcept
    {
        x *= _Row1.x;
        y *= _Row1.y;
        z *= _Row1.z;
    }

    constexpr TOOLBOX_INLINE void operator/=(const Vector3& _Row1) noexcept
    {
        x /= _Row1.x;
        y /= _Row1.y;
        z /= _Row1.z;
    }

    Vector3 operator+(DataType value)
    {
        return {x + value, y + value, z + value};
    }

    Vector3 operator-(DataType value)
    {
        return {x - value, y - value, z - value};
    }

    Vector3 operator*(DataType value)
    {
        return {x * value, y * value, z * value};
    }

    Vector3 operator/(DataType value)
    {
        return {x / value, y / value, z / value};
    }

    TOOLBOX_INLINE DataType& operator[](size_t index)
    {
        return *static_cast<T*>(&x + index);
    }

    TOOLBOX_INLINE const DataType& operator[](size_t index) const
    {
        return *static_cast<const T*>(&x + index);
    }

    TOOLBOX_INLINE bool operator==(const Vector3& _other) const
    {
        return x == _other.x && y == _other.y && z == _other.z;
    }

    TOOLBOX_INLINE bool operator!=(const Vector3& _other)
    {
        return !(this == _other);
    }


    static constexpr Vector3 Zero() { return Vector3(static_cast<T>(0)); };

    static constexpr Vector3 UnitX() { return {1, 0, 0}; };

    static constexpr Vector3 UnitY() { return {0, 1, 0}; };

    static constexpr Vector3 UnitZ() { return {0, 0, 1}; };

    std::string ToString() const
    {
        return "X : " + std::to_string(x) + ", Y : " + std::to_string(y) + ", Z : " + std::to_string(z) + '\n';
    }
};
