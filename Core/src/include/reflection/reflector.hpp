#pragma once
#include "core_header.hpp"
#include <cassert>

#include <functional> // For std::function
#include <iostream>
#include <optional>

#include "log.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflection_typedef.hpp"


BEGIN_PCCORE

using CreateFunc = void (*)(void*);
using DeleteFunc = void (*)(void*);
using SerializeFunc = std::string (*)(const void*);

enum MemberEnumFlag
{
    NONE,
    NOTSERIALIZE,
    COLOR,
    
};


struct Members
{
    DataNature dataNature;
    std::string membersName;
    size_t offset = 0;
    uint32_t typeKey = 0;
    uintmax_t enumFlag = 0;
};
    
struct ReflectedType
{
    uint32_t HashKey;
    DataNature dataNature;
    std::string name;
    size_t dataSize;
    std::vector<Members> members;
    // Dont Support MultiHirietence
    std::vector<uint32_t> inheritenceKey;
    
    CreateFunc createFunc = nullptr;
    DeleteFunc deleteFunc = nullptr;
    
    SerializeFunc serializeFunc = nullptr;     
};


class Reflector
{
public:
    Reflector() = delete;

    ~Reflector() = delete;

    template<typename T>
    static const ReflectedType& GetType();

    template<typename T>
    static uint32_t GetKey();

    static const ReflectedType& GetType(uint32_t _hash);

    template<typename Holder, typename MemberType, MemberEnumFlag enumFlag = NONE>
    static Members ReflectMember(size_t _offset, const char* _memberName);

    template<typename Holder, typename BaseClass = void>
    static ReflectedType* ReflectType();

    template <typename T>
    static uint32_t GetHash();
    
    template <typename T>
    static std::vector<const ReflectedType*> GetAllTypesFrom();

    PC_CORE_API static bool IsTrivial(DataNature _data); 
    
private:
    template <typename T>
    static DataNature TypeToDataNature();
    
    static uint32_t KR_v2_hash(const char *s);
    
    static std::string GetCorrectNameFromTypeId(const std::string& _name);
   
    static inline std::unordered_map<uint32_t, ReflectedType> m_RelfectionMap;

    template <typename  T>
    static void AddType();
    
    template <typename T>
    static uint32_t GetHash(std::string* _name);

    template <typename T>
    static bool ContaintType();

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


template <typename Holder, typename MemberType, MemberEnumFlag memberEnumFlag = NONE>
Members Reflector::ReflectMember(size_t _offset, const char* _memberName)
{
    if (!ContaintType<Holder>())
    {
        PC_LOGERROR("ReflectMember Holder member not found");
        return {};
    }
    
    if (!ContaintType<MemberType>())
    {
        AddType<MemberType>();
    }
    // Add to sub member
    const Members members =
        {
        .dataNature = TypeToDataNature<MemberType>(),
        .membersName = _memberName,
        .offset = _offset,
        .typeKey = GetHash<MemberType>(),
        .enumFlag = memberEnumFlag
        };

    
    m_RelfectionMap.at(GetHash<Holder>()).members.push_back(members);
    return members;
}

template <typename Holder, typename BaseClass = void>
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
void Reflector::AddType()
{
    if (!ContaintType<T>())
    {
        // Create New Node in map
        std::string name;
        const uint32_t hashCode = GetHash<T>(&name);
        const ReflectedType mememberMetaData =
            {
            .HashKey = hashCode,
            .dataNature = TypeToDataNature<T>(),
            .name = name,
            .dataSize = sizeof(T),
            .members = {},
            .createFunc = &ReflectedCreateFunc<T>,
            .deleteFunc = &ReflectedDeleteFunc<T>,
            };
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

    if constexpr (std::is_same_v<T, bool>)
    {
        type = DataNature::BOOL;
    }
    else if constexpr (std::is_same_v<T, int>)
    {
        type = DataNature::INT;
    }
    else if constexpr (std::is_same_v<T, Tbx::Vector2i>)
    {
        type = DataNature::VEC2I;
    }
    else if constexpr (std::is_same_v<T, Tbx::Vector3i>)
    {
        type = DataNature::VEC3I;
    }
    else if constexpr (std::is_same_v<T, uint32_t>)
    {
        type = DataNature::UINT;
    }
    else if constexpr (std::is_same_v<T, float>) 
    {
        type = DataNature::FLOAT;
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        type = DataNature::DOUBLE;
    }
    else if constexpr (std::is_same_v<T, Tbx::Vector2f>)
    {
        type = DataNature::VEC2;
    }
    else if constexpr (std::is_same_v<T, Tbx::Vector3f>)
    {
        type = DataNature::VEC3;
    }
    else if constexpr (std::is_same_v<T, Tbx::Vector4f>)
    {
        type = DataNature::VEC4;
    }
    else if constexpr (std::is_same_v<T, Tbx::Quaternionf>)
    {
        type = DataNature::QUAT;
    }
    else if constexpr (std::is_same_v<T, Tbx::Quaternionf>)
    {
        type = DataNature::INT;
    }
    else if constexpr (std::is_class_v<T>)
    {
        type = DataNature::COMPOSITE;
    }
    else if constexpr (std::is_same_v<T, std::vector<T>>)
    {
        type = DataNature::CONTAINER;
    }
    else
    {
        type = DataNature::UNKNOW;
        static_assert("NotSupported type");
    }

    return type; 
}


#define REFLECT(CurrentType, ...) \
inline ReflectedType* reflectInfo##CurrentType = Reflector::ReflectType<CurrentType, ##__VA_ARGS__>();\

#define REFLECT_MEMBER(CurrentType, memberName, ...) \
inline Members CurrentType##_##memberName##_reflected = Reflector::ReflectMember<CurrentType, decltype(CurrentType::memberName),##__VA_ARGS__>(offsetof(CurrentType, memberName), #memberName);\

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


