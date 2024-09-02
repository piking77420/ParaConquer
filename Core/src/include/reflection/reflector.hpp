#pragma once
#include "core_header.hpp"
#include <functional> // For std::function

#include "log.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflection_typedef.hpp"


BEGIN_PCCORE

using CreateFunc = void (*)();
using DeleteFunc = void (*)();

struct Members
{
    size_t offset;
    uint32_t key;
};
    
struct ReflectedType
{
    DataType dataNature;
    std::string name;
    size_t dataSize;
    std::vector<Members> membersKey;
};

struct ComponentBackend
{
    CreateFunc createFunc = nullptr;
    DeleteFunc deleteFunc = nullptr;
};

class Reflector
{
public:
    Reflector() = delete;

    ~Reflector() = delete;

    template<typename T>
    static const ReflectedType& GetType();

    static const ReflectedType& GetTypeFromHash(uint32_t _hash);

    template<typename Holder, typename MemberType>
    static Members ReflectEntry(size_t _offset, const char* _holderName, const char* _memberName);

    
private:
    template <typename T>
    static DataType TypeToDataType();
    
    static uint32_t KR_v2_hash(const char *s);
    
    static std::string GetCorrectNameFromTypeId(const std::string& _name);
   
    static inline std::unordered_map<uint32_t, ReflectedType> m_RelfectionMap;
};



template <typename T>
const ReflectedType& Reflector::GetType()
{
    const uint32_t key = KR_v2_hash(GetCorrectNameFromTypeId(typeid(T).name()).c_str());
    
    return m_RelfectionMap.at(key);
}

template <typename Holder, typename MemberType>
Members Reflector::ReflectEntry(size_t _offset, const char* _holderName, const char* _memberName)
{
    const std::string holderNameS = GetCorrectNameFromTypeId(_holderName);
    const std::string memberNameS = GetCorrectNameFromTypeId(_memberName);

    const uint32_t hashCodeHolder = KR_v2_hash(holderNameS.c_str());
    bool containType = m_RelfectionMap.contains(hashCodeHolder);
    
    if (!containType)
    {
        // Create New Node in map
        const ReflectedType holderData =
            {
            .dataNature = TypeToDataType<Holder>(),
            .name = holderNameS,
            .dataSize = sizeof(Holder),
            .membersKey = {}
            };
        
        m_RelfectionMap.insert({hashCodeHolder,holderData});
    }

    auto holderData = m_RelfectionMap.at(hashCodeHolder);
    
    // if map not Contain hash then add it for both map and holder vector
    // else just add the hash to the vector holder
    const uint32_t hashMemnber = KR_v2_hash(memberNameS.c_str());
    
    const bool DoesContainMemberType = m_RelfectionMap.contains(hashMemnber);

    
    if (!DoesContainMemberType)
    {
        // Create New Node in map
        const ReflectedType mememberMetaData =
            {
            .dataNature = TypeToDataType<MemberType>(),
            .name = memberNameS,
            .dataSize = sizeof(MemberType),
            .membersKey = {},
            };
        m_RelfectionMap.insert({hashMemnber,mememberMetaData});
    }
    // ADD to the holder types
    auto* it = &m_RelfectionMap.at(hashCodeHolder);
    const Members newMember =
        {
        .offset = _offset,
        .key = hashMemnber
        };
    
    it->membersKey.push_back({_offset, hashMemnber});

    return {};
}


template <typename T>
DataType Reflector::TypeToDataType()
{
    DataType type = {};
    
    if constexpr (std::is_same_v<T, int>)
    {
        type = DataType::INT;
    }
    if constexpr (std::is_same_v<T, Tbx::Vector2i>)
    {
        type = DataType::VEC2I;
    }
    if constexpr (std::is_same_v<T, Tbx::Vector3i>)
    {
        type = DataType::VEC3I;
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
        type = DataType::INT;
    }
    else if constexpr (std::is_class_v<T>)
    {
        type = DataType::COMPOSITE;
    }
    else if constexpr (std::is_same_v<T, std::vector<T>>)
    {
        type = DataType::Vector;
    }
    else
    {
        type = DataType::UNKNOW;
        static_assert("NotSupported type");
    }

    return type; 
}

#define REFLECT(CurrentType, memberName) \
inline Members CurrentType##_##memberName##_reflected = Reflector::ReflectEntry<CurrentType, decltype(CurrentType::memberName)>(offsetof(CurrentType, memberName), typeid(CurrentType).name(), typeid(decltype(CurrentType::memberName)).name());\

END_PCCORE


