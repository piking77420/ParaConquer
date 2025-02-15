#pragma once
#include "core_header.hpp"
#include <cassert>
#include <type_traits>

#include <functional> // For std::function
#include <iostream>
#include <optional>
#include <type_traits>

#include "compiletime_key.hpp"
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


class Reflector
{
public:
    Reflector() = delete;

    ~Reflector() = delete;

    template<typename T>
    static const ReflectedType& GetType();

    template<typename T>
    static bool IsTypeIdIs(TypeId typeId);

    template<typename Base>
    static bool IsBaseOf(const ReflectedType& type);
    
    template <typename T>
    consteval static TypeId GetTypeKey() 
    {
        return COMPILE_TIME_TYPE_KEY(T);
    }

    PC_CORE_API static const ReflectedType& GetType(uint32_t _hash);

    template<typename Holder, typename MemberType, MemberEnumFlag enumFlag = NONE_MEMBER_ENUM_FLAG>
    static Members ReflectMember(size_t _offset, const char* _memberName);

    template<typename Holder, typename BaseClass = void>
    static ReflectedType* ReflectType();
    
    template <typename T>
    static std::vector<const ReflectedType*> GetAllTypesFrom();

    template <typename T>
    static bool isTrivialType();

    PC_CORE_API static bool ContaintTypeFromTypeID(TypeId typeId);

private:
    
    constexpr PC_CORE_API  static std::string GetCorrectNameFromTypeId(const std::string& _name)
    {
        // Remove the nameSpace
        const size_t firstIndex = _name.find_first_of("::",0);
        std::string out;

        if (firstIndex != std::numeric_limits<size_t>::max())
        {
            for (size_t i = firstIndex + 2; i < _name.size(); i++)
                out.push_back(_name[i]); 
        }
        else
        {
            const size_t secondIndex = _name.find_first_of(" ",0);
            if (firstIndex != std::numeric_limits<size_t>::max())
            {
                for (size_t i = secondIndex; i < _name.size(); i++)
                    out.push_back(_name[i]); 
            }
            else
            {
                return _name;
            }
        }
    
        return out; 
    }
   
    PC_CORE_API static inline std::unordered_map<uint32_t, ReflectedType> m_RelfectionMap;

    template <typename  T>
    static void AddType();
    
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

    template <typename T>
    static uintmax_t ProcessMetaData(TypeMetaData* typeMetaData)
    {
        uintmax_t flags = TypeFlagBits::NONE; 
        if constexpr (std::is_class_v<T>)
        {
            flags |= TypeFlagBits::COMPOSITE;
        }
        if constexpr (std::is_pointer_v<T>)
        {
            flags |= TypeFlagBits::PTR;
            typeMetaData->metaDataType = GetType<std::remove_pointer_t<T>>().typeId;
        }
        if constexpr (std::is_array_v<T> || is_vector_v<T> || is_std_array_v<T>)
        {
            flags |=  TypeFlagBits::CONTINUOUS;
        }

        if constexpr (!std::is_abstract_v<T>)
        {
            typeMetaData->createFunc = &ReflectedCreateFunc<T>;
            typeMetaData->deleteFunc = &ReflectedDeleteFunc<T>;
        }

        return flags;
    }
};

template <typename T>
const ReflectedType& Reflector::GetType()
{
    constexpr TypeId tid = GetTypeKey<T>();
    if (!m_RelfectionMap.contains(tid))
        AddType<T>();
    
    return m_RelfectionMap.at(tid);
}

template <typename T>
bool Reflector::IsTypeIdIs(TypeId typeId)
{
    constexpr TypeId tid = GetTypeKey<T>();
    return typeId == tid;
}

template <typename Base>
bool Reflector::IsBaseOf(const ReflectedType& type)
{
    ReflectedType currentType = type;
    ReflectedType baseType = GetType<Base>();

    if (type == baseType)
        return false;

    while (currentType.metaData.baseClass != NullTypeId)
    {
        if (currentType.metaData.baseClass == baseType.typeId)
            return true;

        currentType = GetType(currentType.metaData.baseClass);
    }

    return false;
}


template <typename Holder, typename MemberType, MemberEnumFlag memberEnumFlag>
Members Reflector::ReflectMember(size_t _offset, const char* _memberName)
{
    std::unordered_map<uint32_t, ReflectedType>& memberMap = m_RelfectionMap;
    
    if (!ContaintType<Holder>())
    {
        PC_LOGERROR("ReflectMember Holder member not found")
        return {};
    }
    
    if (!ContaintType<MemberType>())
    {
        AddType<MemberType>();
    }
    const uint32_t holderKey = GetTypeKey<Holder>();
    for(auto&& member :  memberMap.at(holderKey).metaData.members)
    {
        // is there aldready a member name as
        if (member.membersName == _memberName)
            return member;
        
    }
    
    // Add to sub member
    const Members members =
        {
        .typeKey = GetTypeKey<MemberType>(),
        .membersName = _memberName,
        .offset = _offset,
        .memberFlag = memberEnumFlag
        };

    
    memberMap.at(holderKey).metaData.members.push_back(members);
    return members;
}

template <typename Holder, typename BaseClass>
ReflectedType* Reflector::ReflectType()
{

    
    //static_assert(GetTypeKey<Holder>() != GetTypeKey<BaseClass>(), "What are you doing m8");
    //static_assert(GetTypeKey<Tbx::Vector3f>() != GetTypeKey<Tbx::Vector3d>(), "What are you doing m8");


    uint32_t KeyHolder = GetTypeKey<Holder>();

    if (ContaintType<Holder>())
    {
        return &m_RelfectionMap.at(KeyHolder);
    }


    AddType<Holder>();

    // Add base class to current class and all hieritance
    if constexpr(!std::is_same_v<void, BaseClass> || std::is_base_of_v<BaseClass, Holder>)
    {
        if (!ContaintType<BaseClass>())
        {
            AddType<BaseClass>();
        }

        auto it = m_RelfectionMap.find(KeyHolder);
        const ReflectedType& baseType = Reflector::GetType<BaseClass>();

        if (it != m_RelfectionMap.end())
        {
            it->second.metaData.baseClass = baseType.typeId;
            
            it->second.metaData.members.insert(
                it->second.metaData.members.end(),
                baseType.metaData.members.begin(),
                baseType.metaData.members.end()
            );        }
        
    }

    return &m_RelfectionMap.at(KeyHolder);
}


template <typename T>
std::vector<const ReflectedType*> Reflector::GetAllTypesFrom()
{
    constexpr uint32_t hashCode = GetTypeKey<T>();
    std::vector<const ReflectedType*> types;

    for (auto& type : m_RelfectionMap)
    {
        if (IsBaseOf<T>(type.second))
        {
            types.push_back(&type.second);
        }
    }

    return types;
}

template <typename T>
bool Reflector::isTrivialType()
{
    return !(GetType<T>().typeFlags & TypeFlagBits::COMPOSITE);
}


template <typename T>
void Reflector::AddType()
{
    if (!ContaintType<T>())
    {
        // Create New Node in map
        std::string name = GetCorrectNameFromTypeId(typeid(T).name());

        TypeId typeId = GetTypeKey<T>();
        
        ReflectedType type =
            {
            .typeId = typeId,
            .typeFlags = {},
            .name = name,
            .size = sizeof(T),
            .metaData = {}
            };

        type.typeFlags = ProcessMetaData<T>(&type.metaData);
        m_RelfectionMap.insert({typeId,type});
    }
}


template <typename T>
bool Reflector::ContaintType()
{
    return ContaintTypeFromTypeID(GetTypeKey<T>());
}






#define REFLECT(CurrentType, ...) \
static inline PC_CORE::ReflectedType* reflectInfo##CurrentType = PC_CORE::Reflector::ReflectType<CurrentType, ##__VA_ARGS__>();\


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


