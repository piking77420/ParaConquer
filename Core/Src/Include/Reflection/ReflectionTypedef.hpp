    #pragma once
#include "CoreHeader.hpp"
#include <Vector>
#include <String>
#include <Map>
#include <Bitset>

#include "DataStructure/SpareSet.hpp"

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
    SERIALIZE,
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
    WeakPtr,
    SharedPtr,
    String,
    Array,
    Vector,
    Map,
    UnordoredMap,
    BitSet,
    Set,
    SparseSet,
    FileSystem,
};

struct ReflectedBitSet
{
};

struct WeakPtr
{
    TypeId type;
};
struct SharedPtr
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
    uint32_t offsetBetweenKeyAndValueInPair;
};
struct ReflectMapFunction
{
    uint64_t reserveFunction;
    uint64_t insertFunction;
    uint64_t unrefFunc;
    uint64_t incrementFunc;
};



struct Set
{
    TypeId type;
};

struct ReflectedSparset
{
    TypeId denseVector;
    uint32_t denseVectorOffSet;
    TypeId spareVector;
    uint32_t spareVectorOffset;
};

struct TypeNatureMetaData
{
    TypeNatureMetaDataEnum metaDataTypeEnum = TypeNatureMetaDataEnum::None;
    union TypeNatureMetaUnion
    {
        WeakPtr weakPtr;
        SharedPtr sharedPtr;
        RelfectedString relfectedString;
        Array array;
        Vector vector;
        ReflectedMap mapReflected;
        ReflectedBitSet bitSet;
        ReflectedSparset reflectedSparset;

    }metaDataType;
};


struct TypeMetaData
{
    TypeNatureMetaData typeNatureMetaData;
    std::vector<Members> members;

    // Dont Support MultiHirietence
    // TODO Support MultiHirietence exemple handle interface
    TypeId baseClass = NullTypeId;
    
    CreateFunc createFunc = nullptr;
    DeleteFunc deleteFunc = nullptr;
    HashFunc hashFun;
};

struct ReflectedType
{
    TypeId typeId{};
    uintmax_t typeFlags{};
    
    std::string name{};
    size_t size{};
    uint32_t alignment{};
    
    TypeMetaData metaData{};
    size_t rttiTypeId{};

    DEFAULT_COPY_MOVE_OPERATIONS(ReflectedType)

    DEFAULT_CONSTRUCTOR_DESTRUCTOR(ReflectedType)
    
    bool operator==(const ReflectedType& other) const
    {
        return typeId == other.typeId && rttiTypeId == other.rttiTypeId;
    }

    const Members* GetMemberByName(const std::string& _memberName) const
    {
        auto it  = std::find_if(metaData.members.begin(), metaData.members.end(),[&_memberName](const Members& _m)
        {
            return _m.membersName == _memberName;
        });

        return it == metaData.members.end() ? nullptr : &(*it);
    }


};

template<typename T>
struct is_vector : std::false_type {};

template<typename T>
struct is_vector<std::vector<T>> : std::true_type {};

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

template <typename>
struct is_bit_set : std::false_type {};

template <size_t _size>
struct is_bit_set<std::bitset<_size>> : std::true_type {};

template <typename>
struct is_sparse_set : std::false_type {};

template <typename T>
struct is_sparse_set<SpareSet<T>> : std::true_type {};


template <typename T>
std::size_t HashFunction(const void* obj) {
    return std::hash<T>{}(*static_cast<const T*>(obj));
}




END_PCCORE