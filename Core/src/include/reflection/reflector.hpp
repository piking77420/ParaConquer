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

    template<typename T>
    static uint32_t GetKey();

    static const ReflectedType& GetTypeFromHash(uint32_t _hash);

    template<typename Holder, typename MemberType>
    static Members ReflectMember(size_t _offset, const char* _holderName, const char* _memberName);

    template<typename Holder>
    static ReflectedType* ReflectType();
    
private:
    template <typename T>
    static DataType TypeToDataType();
    
    static uint32_t KR_v2_hash(const char *s);
    
    static std::string GetCorrectNameFromTypeId(const std::string& _name);
   
    static inline std::unordered_map<uint32_t, ReflectedType> m_RelfectionMap;
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



template <typename Holder, typename MemberType>
Members Reflector::ReflectMember(size_t _offset, const char* _holderName, const char* _memberName)
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

template <typename Holder>
ReflectedType* Reflector::ReflectType()
{
    const std::string holderNameS = GetCorrectNameFromTypeId(typeid(Holder).name());
    const uint32_t hashCodeHolder = KR_v2_hash(holderNameS.c_str());

    const bool containType = m_RelfectionMap.contains(hashCodeHolder);

    if (containType)
        return &m_RelfectionMap.at(hashCodeHolder);
    
    const ReflectedType holderData =
        {
        .dataNature = TypeToDataType<Holder>(),
        .name = holderNameS,
        .dataSize = sizeof(Holder),
        .membersKey = {}
        };
        
    m_RelfectionMap.insert({hashCodeHolder,holderData});

    return &m_RelfectionMap.at(hashCodeHolder);
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

#define REFLECT(CurrentType) \
inline ReflectedType* reflectInfo#CurrentType = Reflector::ReflectType<CurrentType>();\

#define REFLECT_MEMBER(CurrentType, memberName) \
inline Members CurrentType##_##memberName##_reflected = Reflector::ReflectMember<CurrentType, decltype(CurrentType::memberName)>(offsetof(CurrentType, memberName), typeid(CurrentType).name(), typeid(decltype(CurrentType::memberName)).name());\

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


