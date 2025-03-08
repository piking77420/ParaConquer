    #pragma once
#include "core_header.hpp"
#include <vector>
#include <string>
#include <map>

BEGIN_PCCORE

using TypeId = uint32_t; 
static constexpr TypeId NullTypeId = 0;
using TypeFlag = uintmax_t;

using CreateFunc = void (*)(void*);
using DeleteFunc = void (*)(void*);
using SerializeFunc = std::string (*)(const void*);
using DerializeFunc = void (*)(void*, const std::string&);
using OnEditFunc = void (*)(void*);
using HashFunc = std::size_t(*)(const void*);
 
enum TypeFlagBits
{
  NONE       = 0,        // No flags set
  COMPOSITE  = 1 << 0,  // Composite of trivial type
};




enum MemberEnumFlag
{
    NONE_MEMBER_ENUM_FLAG,
    NOTSERIALIZE,
    COLOR,
    HIDE_INSPECTOR,
};


struct Members
{
    TypeId typeKey = NullTypeId;
    std::string membersName;
    size_t offset = 0;
    uintmax_t memberFlag = 0;
};



enum struct TypeNatureMetaDataEnum
{
    None,
    ResourceRefType,
    ResourceHandle,
    String,
    Array,
    Vector,
    Map,
    UnordoredMap,
};

struct ResourceRefType
{
    TypeId type;
};
struct ResourceHandleType
{
    TypeId type;
};

struct Array
{
    TypeId type;
    size_t size;
};

struct RelfectedString
{
    TypeId type;
};

struct Vector
{
    TypeId type;
};

struct ReflectedMap
{   
    TypeId key;
    TypeId value;
};

struct Set
{
    TypeId type;
};

struct TypeNatureMetaData
{
    TypeNatureMetaDataEnum metaDataTypeEnum;
    union TypeNatureMetaUnion
    {
        ResourceRefType resourceRef;
        ResourceHandleType resourceHandleType;
        RelfectedString relfectedString;
        Array array;
        Vector vector;
        ReflectedMap mapReflected;
        ReflectedMap unordoredMapReflected;

    }metaDataType;
};


struct TypeMetaData
{
    TypeNatureMetaData typeNatureMetaData;
    std::vector<Members> members;

    // Dont Support MultiHirietence
    TypeId baseClass = NullTypeId;
    
    CreateFunc createFunc = nullptr;
    DeleteFunc deleteFunc = nullptr;
    HashFunc hashFun;
};

struct ReflectedType
{
    TypeId typeId;
    uintmax_t typeFlags;
    
    std::string name;
    size_t size;
    
    TypeMetaData metaData;

    bool operator==(const ReflectedType& other) const
    {
        return typeId == other.typeId;
    }
};

// Chat gpt
template<typename T>
struct is_vector : std::false_type {};

// Specialization (for vectors)
template<typename T>
struct is_vector<std::vector<T>> : std::true_type {};

// Convenience variable template
template<typename T>
inline constexpr bool is_vector_v = is_vector<std::decay_t<T>>::value;

template<typename T>
struct is_std_array : std::false_type {};

template<typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template<typename T>
inline constexpr bool is_std_array_v = is_std_array<std::decay_t<T>>::value;

template <typename>
struct is_weak_ptr : std::false_type {};

template <typename U>
struct is_weak_ptr<std::weak_ptr<U>> : std::true_type {};

template <typename T>
inline constexpr bool is_weak_ptr_v = is_weak_ptr<T>::value;

template <typename>
struct is_shared_ptr : std::false_type {};

template <typename U>
struct is_shared_ptr<std::shared_ptr<U>> : std::true_type {};

template <typename T>
inline constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

template <typename>
struct is_map : std::false_type {};

template <typename Key, typename Value, typename... Args>
struct is_map<std::map<Key, Value, Args...>> : std::true_type {};

template <typename>
struct is_unordered_map : std::false_type {};

template <typename Key, typename Value, typename... Args>
struct is_unordered_map<std::unordered_map<Key, Value, Args...>> : std::true_type {};

template <typename T>
std::size_t HashFunction(const void* obj) {
    return std::hash<T>{}(*static_cast<const T*>(obj));
}

struct ReflectMapFunction
{
    uint64_t reserveFunction;
    uint64_t insertFunction;
    uint64_t unrefFunc;
    uint64_t incrementFunc;

};

END_PCCORE