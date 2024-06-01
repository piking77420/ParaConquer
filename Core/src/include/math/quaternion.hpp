#pragma once
#include <string>

#include "vector3.hpp"

template <typename T>
class Quaternion
{
public:
    using DataType = T; 
    using Vec3 = Vector3<DataType>; 

    Vec3 imaginary = Vec3::Zero();
    DataType real = static_cast<T>(0);

    Quaternion() = default;

    Quaternion(const Vec3& _vec, const DataType _real) : imaginary(_vec) , real(_real)
    {
        
    }

    Quaternion(const DataType _x, const DataType _y, const DataType _z, const DataType _real) : imaginary( {_x, _y, _z} ) , real(_real)
    {
        
    }

    ~Quaternion() = default;

    constexpr static TOOLBOX_INLINE Quaternion Identity() { return {Vec3::Zero(), static_cast<DataType>(1) };}

    DataType MagnitudeSquare() const 
    {
        return imaginary.Magnitude() + real * real;
    }
    
    DataType Magnitude() const 
    {
        return std::sqrt(MagnitudeSquare());
    }

    Quaternion Conjugate() const
    {
        return {imaginary, -real};
    }
    
    static constexpr DataType DotProduct(const Quaternion& q1, const Quaternion& q2)
    {
        return q1.imaginary.x * q2.imaginary.x + q1.imaginary.y * q2.imaginary.y + q1.imaginary.z * q2.imaginary.z + q1.real * q2.real;
    }
    
    Quaternion Normalize()
    {
        const DataType mag = Magnitude();

        if (IsEqualTo<DataType>(mag,static_cast<T>(1)))
            return this;

        const DataType InvMagnitude = static_cast<T>(1) / mag;
        
        return { imaginary.x * InvMagnitude, imaginary.y * InvMagnitude, imaginary.z * InvMagnitude, real * InvMagnitude};    
    }

    Quaternion Inverse() const
    {
        return Conjugate() / MagnitudeSquare();
    }

    Quaternion LookAt(Vec3 _sourcePoint, Vec3 _destPoint,const Vec3 forward) const
    {
        Vec3 forwardVector = (_destPoint - _sourcePoint).Normalize();

        Vec3 rotAxis = Vec3::Cross(forward, forwardVector);
        DataType dot = Vec3::Dot(forward, forwardVector);

        Quaternion q;
        q.imaginary.x = rotAxis.x;
        q.imaginary.y = rotAxis.y;
        q.imaginary.z = rotAxis.z;
        q.real = dot + static_cast<DataType>(1);

        return {rotAxis, dot + 1};
    }

    Quaternion operator+(const Quaternion& _other)
    {
        return {imaginary + _other.imaginary, real + _other.real};
    }

    Quaternion operator-(const Quaternion& _other)
    {
        return {imaginary - _other.imaginary, real - _other.real};
    }

    Quaternion operator*(const Quaternion& _other)
    {
        const DataType newReal = real * _other.real
        - decltype(imaginary)::Dot(imaginary, _other.imaginary);

        const DataType i = imaginary.x * _other.real + real * _other.imaginary.x
        - imaginary.z * _other.imaginary.y + imaginary.y * _other.imaginary.z;

        const DataType j = imaginary.y * _other.real + imaginary.z * _other.imaginary.x
        + real * _other.imaginary.y - imaginary.x * _other.imaginary.z; 

        const DataType k = imaginary.z * _other.real - imaginary.y * _other.imaginary.x
        + imaginary.x * _other.imaginary.y + real * _other.imaginary.z;
        
        return { { i, j, k}, newReal};
    }

    Quaternion operator+(float _value)
    {
        return {imaginary + _value, real + _value};
    }
    
    Quaternion operator-(float _value)
    {
        return {imaginary - _value, real - _value};
    }
    
    Quaternion operator*(float _value)
    {
        return {imaginary * _value, real * _value};
    }
    
    Quaternion operator/(float _value)
    {
        return {imaginary / _value, real / _value};
    }

    void operator+=(const Quaternion& _other)
    {
        *this = *this + _other;
    }
    
    void operator-=(const Quaternion& _other)
    {
        *this = *this - _other;
    }
    
    void operator*=(const Quaternion& _other)
    {
        *this = this * _other;
    }

    void operator+=(float _value)
    {
        *this = *this + _value; 
    }
    
    void operator-=(float _value)
    {
        *this = *this - _value;
    }
    
    void operator*=(float _value)
    {
        *this = *this * _value;
    }
    
    void operator/=(float _value)
    {
        *this = *this / _value;
    }

    Vec3 ToEulerAngles() const
    {
        DataType y = std::atan2(2.f * imaginary.x * real + 2.f * imaginary.y * imaginary.z, 1 - 2.f * (imaginary.z * imaginary.z + real * real));
        DataType x = std::asin(2.f * (imaginary.x * imaginary.z - real * imaginary.y));
        DataType z = std::atan2(2.f * imaginary.x * imaginary.y + 2.f * imaginary.z * real, 1 - 2.f * (imaginary.y * imaginary.y + imaginary.z * imaginary.z));

        return {x, y, z};
    }

    static Quaternion FromEuleur(const Vec3& eulerAngle)
    {
        Vec3 c;
        Vec3 s;
        
        if constexpr (std::is_floating_point_v<T>)
        {
            c = Vector3(std::cos(eulerAngle.x * 0.5f), std::cos(eulerAngle.y * 0.5f), std::cos(eulerAngle.z * 0.5f));
            s = Vector3(std::sin(eulerAngle.x * 0.5f), std::sin(eulerAngle.y * 0.5f), std::sin(eulerAngle.z * 0.5f));
        }
        else
        {
            c = Vector3(std::cos(eulerAngle.x / static_cast<T>(2)), std::cos(eulerAngle.y / static_cast<T>(2)), std::cos(eulerAngle.z / static_cast<T>(2)));
            s = Vector3(std::sin(eulerAngle.x / static_cast<T>(2)), std::sin(eulerAngle.y / static_cast<T>(2)), std::sin(eulerAngle.z / static_cast<T>(2)));
        }
        
        DataType i = s.x * c.y * c.z + c.x * s.y * s.z;
        DataType j = c.x * s.y * c.z - s.x * c.y * s.z;
        DataType k = c.x * c.y * s.z + s.x * s.y * c.z;
        DataType w = c.x * c.y * c.z - s.x * s.y * s.z;

        return {{ i, j, k},w};
    }

    template<typename U>
    [[nodiscard]] static TOOLBOX_INLINE Quaternion Slerp(const Quaternion& t1, const Quaternion& t2, U t)
    {
        float cosOmega = DotProduct(t1, t2);
	
        bool flip = false;

        if (cosOmega < -std::numeric_limits<U>::epsilon())
        {
            flip = true;
            cosOmega = -cosOmega;
        }

        float s1, s2;

        if (cosOmega > (1.0f - std::numeric_limits<U>::epsilon()))
        {
            // Too close, do straight linear interpolation.
            s1 = 1.0f - t;
            s2 = (flip) ? -t : t;
        }
        else
        {
            float omega = std::cos(cosOmega);
            float invSinOmega = 1.f / std::sin(omega);

            s1 = std::sin((1.0f - t) * omega) * invSinOmega;
            s2 = (flip)
                ? -std::sin(t * omega) * invSinOmega
                : std::sin(t * omega) * invSinOmega;
        }

        Quaternion ans;
        ans.imaginary.x = s1 * t1.imaginary.x + s2 * t2.imaginary.x;
        ans.imaginary.y = s1 * t1.imaginary.y + s2 * t2.imaginary.y;
        ans.imaginary.z = s1 * t1.imaginary.z + s2 * t2.imaginary.z;
        ans.real = s1 * t1.real + s2 * t2.real;

        return ans;
    }

    [[nodiscard]]
    std::string ToString() const
    {
        return "X : " + std::to_string(imaginary.x) + ", Y : " + std::to_string(imaginary.y) + ", Z : " + std::to_string(imaginary.z) + ", W : " + std::to_string(real) + '\n';
    }
};
