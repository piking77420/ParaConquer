#pragma once
#include <cassert>

#include "world/component.hpp"
#include "core_header.hpp"
#include <functional> // For std::function

#include "math/toolbox_typedef.hpp"
#include "reflection/reflection_typedef.hpp"

BEGIN_PCCORE
class Reflector
{
public:
    
    Reflector() = default;

    ~Reflector() = default;

    template<typename ComponentType, typename MemberType>
    static Reflector Create(size_t _size, size_t _offset, const char* _name);

    
private:
    static void RegisterInternal(uint32_t _componentID, size_t _size, size_t _offset, DataType _datatype, const char* _name);

    template <typename T>
    static DataType TypeToDataType();
};

template <typename ComponentType, typename MemberType>
Reflector Reflector::Create(size_t _size, size_t _offset, const char* _name)
{
    const Reflector reflected = {};
    const DataType datatype  = TypeToDataType<MemberType>();

    reflected.RegisterInternal(ComponentHandle<ComponentType>::componentID,_size, _offset, datatype , _name);
    return reflected;
}

template <typename T>
DataType Reflector::TypeToDataType()
{
    DataType type = {};

    if constexpr (std::is_same_v<T, bool>)
    {
        type = DataType::BOOL;
    }
    else if constexpr (std::is_same_v<T, int>)
    {
        type = DataType::INT;
    }
    else if constexpr (std::is_same_v<T, uint32_t>)
    {
        type = DataType::UINT;
    }
    else if constexpr (std::is_same_v<T, float>) 
    {
        type = DataType::FLOAT;
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        type = DataType::DOUBLE;
    }
    else if constexpr (std::is_same_v<T, Tbx::Vector2f>)
    {
        type = DataType::VEC2;
    }
    else if constexpr (std::is_same_v<T, Tbx::Vector3f>)
    {
        type = DataType::VEC3;
    }
    else if constexpr (std::is_same_v<T, Tbx::Vector4f>)
    {
        type = DataType::VEC4;
    }
    else if constexpr (std::is_same_v<T, Tbx::Quaternionf>)
    {
        type = DataType::QUAT;
    }
    else if constexpr (std::is_same_v<T, Tbx::Quaternionf>)
    {
        type = DataType::INT;
    }
    else
    {
        assert("NotSupportedType");
    }

    return type; 
}


#define REFLECT(currentComponent, memberName) \
inline Reflector currentComponent##_##memberName##_reflected = Reflector::Create<currentComponent, decltype(currentComponent::memberName)>(sizeof(currentComponent::memberName), offsetof(currentComponent, memberName), #memberName);\

END_PCCORE


