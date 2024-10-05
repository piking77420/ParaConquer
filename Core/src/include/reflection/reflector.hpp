#pragma once
#include "core_header.hpp"
#include <cassert>
#include <type_traits>

#include <functional> // For std::function
#include <iostream>
#include <optional>

#include "log.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflection_typedef.hpp"



BEGIN_PCCORE

// Chat gpt
template<typename T>
struct is_vector : std::false_type {};

// Specialization (for vectors)
template<typename T>
struct is_vector<std::vector<T>> : std::true_type {};

// Convenience variable template
template<typename T>
inline constexpr bool is_vector_v = is_vector<std::decay_t<T>>::value;

// Primary template (for all types except std::array)
template<typename T>
struct is_std_array : std::false_type {};

// Specialization for std::array
template<typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

// Convenience variable template
template<typename T>
inline constexpr bool is_std_array_v = is_std_array<std::decay_t<T>>::value;


#define IsTypeOfOrSubType(x)\
std::is_same_v<T, x> || std::is_same_v<std::remove_all_extents_t<T>, x>\

class Reflector
{
public:
    Reflector() = delete;

    ~Reflector() = delete;

    template<typename T>
    static const ReflectedType& GetType();

    template<typename T>
    static uint32_t GetKey();

    template <typename T>
    static TypeInfo GetTypeInfo();

    PC_CORE_API static const ReflectedType& GetType(uint32_t _hash);

    template<typename Holder, typename MemberType, MemberEnumFlag enumFlag = NONE>
    static Members ReflectMember(size_t _offset, const char* _memberName);

    template<typename Holder, typename BaseClass = void>
    static ReflectedType* ReflectType();

    template <typename T>
    static uint32_t GetHash();
    
    template <typename T>
    static std::vector<const ReflectedType*> GetAllTypesFrom();

    template <typename T>
    static bool isTrivialType();

private:
    
    PC_CORE_API static uint32_t KR_v2_hash(const char *s);
    
    PC_CORE_API static std::string GetCorrectNameFromTypeId(const std::string& _name);
   
    PC_CORE_API static inline std::unordered_map<uint32_t, ReflectedType> m_RelfectionMap;

    template <typename  T>
    static void AddType();
    
    template <typename T>
    static uint32_t GetHash(std::string* _name);

    template <typename T>
    static bool ContaintType();
    
    template <class T>
    static DataNature TypeToDataNature();

    template <class T>
    static uint32_t GetTypeInfoFlags();

    template <class T>
    static void GetArrayInfoFromType(TypeInfo* _typeInfo);

    template <typename T>
    static void ReflectedCreateFunc(void* _class)
    {
        new (_class) T();
    }

    template <typename T>
    static void ReflectedDeleteFunc(void* _class)
    {
        static_cast<T*>(_class)->~T();
    }
    
};


template <typename T>
uint32_t Reflector::GetKey()
{
    return KR_v2_hash(GetCorrectNameFromTypeId(typeid(T).name()).c_str());
}

template <typename T>
const ReflectedType& Reflector::GetType()
{
    const uint32_t key = GetKey<T>();
    
    return m_RelfectionMap.at(key);
}


template <typename Holder, typename MemberType, MemberEnumFlag memberEnumFlag>
Members Reflector::ReflectMember(size_t _offset, const char* _memberName)
{
    if (!ContaintType<Holder>())
    {
        PC_LOGERROR("ReflectMember Holder member not found")
        return {};
    }
    
    if (!ContaintType<MemberType>())
    {
        AddType<MemberType>();
    }
    // Add to sub member
    const Members members =
        {
        .typeKey = GetHash<MemberType>(),
        .membersName = _memberName,
        .offset = _offset,
        .enumFlag = memberEnumFlag
        };

    
    m_RelfectionMap.at(GetHash<Holder>()).members.push_back(members);
    return members;
}

// to do fix warning with void 
template <typename Holder, typename BaseClass>
ReflectedType* Reflector::ReflectType()
{
    AddType<Holder>();

    // Add base class to current class and all hieritance
    if constexpr(!std::is_same_v<void,BaseClass> || std::is_base_of_v<BaseClass, Holder>)
    {
        if (!ContaintType<BaseClass>())
        {
            AddType<BaseClass>();
        }

        const uint32_t TypeKey = GetKey<Holder>();
        auto it = m_RelfectionMap.find(TypeKey);
        const uint32_t baseKey = GetKey<BaseClass>();

        if (it != m_RelfectionMap.end())
        {
            it->second.inheritenceKey.push_back(baseKey);
            auto& baseClass = m_RelfectionMap.at(baseKey);
            it->second.inheritenceKey.insert(it->second.inheritenceKey.end(), baseClass.inheritenceKey.begin(), baseClass.inheritenceKey.end());
        }
        
    }

    return &m_RelfectionMap.at(GetKey<Holder>());
}

template <typename T>
uint32_t Reflector::GetHash()
{
    const std::string holderNameS = GetCorrectNameFromTypeId(typeid(T).name());
    const uint32_t hashCodeHolder = KR_v2_hash(holderNameS.c_str());
    return hashCodeHolder;
}

template <typename T>
std::vector<const ReflectedType*> Reflector::GetAllTypesFrom()
{
    const uint32_t hashCode = GetHash<T>();
    std::vector<const ReflectedType*> types;

    for (auto it = m_RelfectionMap.begin(); it != m_RelfectionMap.end(); it++)
    {
        for (size_t i = 0; i < it->second.inheritenceKey.size(); i++)
        {
            if (it->second.inheritenceKey[i] == hashCode)
            {
                types.push_back(&it->second);
            }
        }
    }

    return types;
}

template <typename T>
bool Reflector::isTrivialType()
{
    DataNature dataNature = TypeToDataNature<T>();
    return dataNature != DataNature::UNKNOWN && dataNature != DataNature::RESOURCE;
}

template <typename T>
TypeInfo Reflector::GetTypeInfo()
{
    return {TypeToDataNature<T>(), GetTypeInfoFlags<T>()};
}

template <typename T>
void Reflector::AddType()
{
    if (!ContaintType<T>())
    {
        // Create New Node in map
        std::string name;
        const uint32_t hashCode = GetHash<T>(&name);
        ReflectedType mememberMetaData =
            {
            .HashKey = hashCode,
            .typeInfo = GetTypeInfo<T>(),
            .name = name,
            .typeSize = sizeof(T),
            .members = {},
            };

        if constexpr (!std::is_abstract_v<T>)
        {
            mememberMetaData.createFunc = &ReflectedCreateFunc<T>;
            mememberMetaData.deleteFunc = &ReflectedDeleteFunc<T>;
        }

        
        m_RelfectionMap.insert({hashCode,mememberMetaData});
    }
}

template <typename T>
uint32_t Reflector::GetHash(std::string* _name)
{
    *_name = GetCorrectNameFromTypeId(typeid(T).name());
    const uint32_t hashCodeHolder = KR_v2_hash(_name->c_str());
    return hashCodeHolder;
}

template <typename T>
bool Reflector::ContaintType()
{
    return m_RelfectionMap.contains(GetHash<T>());
}



template <typename T>
DataNature Reflector::TypeToDataNature()
{
    DataNature type = {};

    if constexpr (IsTypeOfOrSubType(char))
    {
        type = DataNature::CHAR;
    }
    if constexpr (IsTypeOfOrSubType(bool))
    {
        type = DataNature::BOOL;
    }
    else if constexpr (IsTypeOfOrSubType(int))
    {
        type = DataNature::INT;
    }
    else if constexpr (IsTypeOfOrSubType(Tbx::Vector2i))
    {
        type = DataNature::VEC2I;
    }
    else if constexpr (IsTypeOfOrSubType(Tbx::Vector3i))
    {
        type = DataNature::VEC3I;
    }
    else if constexpr (IsTypeOfOrSubType(uint32_t))
    {
        type = DataNature::UINT;
    }
    else if constexpr (IsTypeOfOrSubType(float)) 
    {
        type = DataNature::FLOAT;
    }
    else if constexpr (IsTypeOfOrSubType(double))
    {
        type = DataNature::DOUBLE;
    }
    else if constexpr (IsTypeOfOrSubType(Tbx::Vector2f))
    {
        type = DataNature::VEC2;
    }
    else if constexpr (IsTypeOfOrSubType(Tbx::Vector3f))
    {
        type = DataNature::VEC3;
    }
    else if constexpr (IsTypeOfOrSubType(Tbx::Vector4f))
    {
        type = DataNature::VEC4;
    }
    else if constexpr (IsTypeOfOrSubType(Tbx::Quaternionf))
    {
        type = DataNature::QUAT;
    }
    else if constexpr (IsTypeOfOrSubType(std::string))
    {
        type = DataNature::STRING;
    }
    else
    {
        type = DataNature::UNKNOWN;
    }

    return type; 
}

template <class T>
uint32_t Reflector::GetTypeInfoFlags()
{
    uint32_t type = 0;

    if constexpr (is_vector_v<T>)
    {
        type = TypeFlag::VECTOR;
    }
    if constexpr (std::is_class_v<T>)
    {
        // Avoid String to be composite
        if (!isTrivialType<T>())
            type |= static_cast<uint32_t>(TypeFlag::COMPOSITE);
    }
    if constexpr (std::is_bounded_array_v<T>)
    {
        type |= static_cast<uint32_t>(TypeFlag::ARRAY);
    }
    if constexpr (std::is_pointer_v<T>)
    {
        type |= static_cast<uint32_t>(TypeFlag::POINTER);
    }
    

    return type;
}

template <class T>
void Reflector::GetArrayInfoFromType(TypeInfo* _typeInfo)
{
    if constexpr (is_vector_v<T>)
    {
        
    }
    else if constexpr (std::is_bounded_array_v<T>)
    {
        
    }
    else
    {
        
    }
    
}


#define REFLECT(CurrentType, ...) \
inline PC_CORE::ReflectedType* reflectInfo##CurrentType = PC_CORE::Reflector::ReflectType<CurrentType, ##__VA_ARGS__>();\


#define REFLECT_MEMBER(CurrentType, memberName, ...) \
inline PC_CORE::Members CurrentType##_##memberName##_reflected = PC_CORE::Reflector::ReflectMember<CurrentType, decltype(CurrentType::memberName),##__VA_ARGS__>(offsetof(CurrentType, memberName), #memberName);\

template <typename Tag>
typename Tag::type saved_private_v;

template <typename Tag, typename Tag::type x>
bool save_private_v = (saved_private_v<Tag> = x);

#define GetPrivateVariable(className, memberName) \
struct Widget_##memberName##_ { \
using type = decltype(&className::memberName); \
}; \
template bool save_private_v<typename Widget_##memberName##_::type, &className::memberName>;\

#define GetPrivateFunc(privateVarMember)\
struct Widget_f_ { using type = int (Widget::*)() const; };\
template bool save_private_v<Widget_f_, &Widget::f_>;\

END_PCCORE


