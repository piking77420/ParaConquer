#pragma once
#include <cassert>
#include <type_traits>

#include <filesystem>
#include <functional> // For std::function
#include <iostream>
#include <optional>


#include "CoreHeader.hpp"
#include "Reflection/ReflectionTypedef.hpp"
#include "CompiletimeKey.hpp"
#include "PerfRegion.hpp"
#include "Log.hpp"

BEGIN_PCCORE

// TODO find a nother way than foward declare
// bet solution so far
class DynamicReflectable;
template<class T>
concept DynamicReflectableDerived = std::is_base_of_v<DynamicReflectable, T>;
template<DynamicReflectableDerived T>
class ObjectPtr;
template<DynamicReflectableDerived T>
class WeakObjectPtr;

template<typename T>
struct is_object_ptr : std::false_type {};

template<typename U>
struct is_object_ptr<ObjectPtr<U>> : std::true_type {};

template<typename T>
struct is_weak_object_ptr : std::false_type {};

template<typename U>
struct is_weak_object_ptr<WeakObjectPtr<U>> : std::true_type {};

template<typename T>
struct is_object_or_weak_ptr
	: std::bool_constant<is_object_ptr<T>::value || is_weak_object_ptr<T>::value> {
};

class Reflector
{
public:
	Reflector() = delete;

	~Reflector() = delete;

	template<typename T>
	static const ReflectedType& GetType();

	template<typename T>
	static const ReflectedEnum& GetEnum();

	PC_CORE_API static const ReflectedType& GetTypeFromRTTI(size_t typeIdFromRtti);

	template<typename T>
	static bool IsTypeIdIs(TypeId typeId);

	template<typename Base>
	static bool IsBaseOf(const ReflectedType& type);

	template <typename T>
	constexpr  static TypeId GetTypeKey()
	{
		return COMPILE_TIME_TYPE_KEY(T);
	}
	
	PC_CORE_API static const ReflectedType& GetType(uint32_t _hash);

	template<typename Holder, typename MemberType, MemberEnumFlag enumFlag = NONE_MEMBER_ENUM_FLAG>
	static uint8_t ReflectMember(size_t _offset, const char* _memberName);

	template<typename Holder, typename BaseClass = void>
	static uint8_t ReflectType();

	template <typename T>
	static std::vector<const ReflectedType*> GetAllTypesFrom();

	template <typename T>
	static bool isTrivialType();

	static bool isTrivialType(TypeId _id);

	PC_CORE_API static bool Containt(TypeId typeId);

	template <typename T, typename F>
	static bool GetPtrToTypeField(T* _object,const std::string& _fieldName, F** _outPtrToField);

	template <typename T, typename F>
	static bool GetPtrToTypeField(const T& _object, const std::string& _fieldName, const F** _outPtrToField);

	PC_CORE_API static bool GetPtrToTypeField(TypeId _id, void* _object,const std::string& _fieldName,  void** _outPtrToField);

	PC_CORE_API static bool GetPtrToTypeField(TypeId _id , const void* _object,const std::string& _fieldName, const void** _outPtrToField);

	PC_CORE_API static inline std::unordered_map<TypeId, ReflectMapFunction> m_MapReflectFunction;

	PC_CORE_API static inline std::unordered_map<TypeId, ReflectMapFunction> m_UnordoredMapReflectFunction;


private:

	constexpr PC_CORE_API static std::string GetCorrectNameFromTypeId(const std::string& _name)
	{
		// Search for "::" to remove namespace
		size_t firstIndex = _name.find("::");
		std::string out;

		if (firstIndex != std::string::npos)
		{
			out = _name.substr(firstIndex + 2);
		}
		else
		{
			// Search for " " to remove "enum ", "class ", etc.
			size_t secondIndex = _name.find(' ');
			if (secondIndex != std::string::npos)
			{
				out = _name.substr(secondIndex + 1);
			}
			else
			{
				return _name; // No namespace or keyword
			}
		}

		return out;
	}

	PC_CORE_API static inline std::unordered_map<TypeId, ReflectedType> m_RelfectionMap;

	PC_CORE_API static inline std::unordered_map<size_t, TypeId> m_RttiToTypeId;


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

		
		if constexpr (is_object_ptr<T>::value)
		{
			ReflectedObjPtr objPtr{GetTypeKey<typename T::Obj>()  };
			typeMetaData->data = objPtr;
		}

		if constexpr (is_weak_object_ptr<T>::value)
		{
			ReflectedWeakObjPtr objPtr{ GetTypeKey<typename T::Obj>()};
			typeMetaData->data = objPtr;
		}

#pragma region ReflectArray

		if constexpr (is_std_array_v<T>)
		{
			ReflectedArray array{ GetTypeKey<typename T::value_type>(), std::tuple_size_v<T> };
			typeMetaData->data = array;
		}
		if constexpr (std::is_array_v<T>)
		{
			ReflectType<std::remove_extent_t<T>>(); 
			ReflectedArray array{ GetTypeKey<std::remove_extent_t<T>>(), std::extent_v<T> };
			typeMetaData->data = array;
		}
#pragma endregion ReflectArray

		if constexpr (is_vector_v<T>)
		{
			ReflectedVector v{ GetTypeKey<typename std::remove_extent<typename T::value_type>::type>() };
			ReflectType<typename std::remove_extent<typename T::value_type>::type>();
			typeMetaData->data = v;
		}

		if constexpr (std::is_same_v<std::string, T> || std::is_same_v<std::wstring, T>)
		{
			bool constexpr isw = std::is_same_v<std::wstring, T>;
			ReflectedString rs{ isw ? GetTypeKey<wchar_t>() : GetTypeKey<char>() };

			typeMetaData->data = rs;
		}


		if constexpr (is_unordered_map<T>::value || is_map<T>::value)
		{
			ReflectedMap rm;
			rm.key = GetTypeKey<typename T::key_type>();
			rm.value = GetTypeKey<typename T::mapped_type>();

			using MapPair = std::pair<typename T::key_type, typename T::mapped_type>;
			rm.offsetBetweenKeyAndValueInPair = offsetof(MapPair, MapPair::second);
			using MapIterator = typename T::iterator;
			using MapConstIterator = typename T::const_iterator;

			using ReseverMapFunction = void (T::*)(size_t);
			using InsertMapFunction = typename T::mapped_type& (T::*)(const typename T::key_type&);
			using UnorderedMapUnrefConstIteratorFunc = const std::pair<const typename T::key_type, typename T::mapped_type>* (MapConstIterator::*)() const;
			using IncrementMapIterator = MapIterator & (MapIterator::*)();


			ReseverMapFunction reserverFunctionMapType = &T::reserve;
			InsertMapFunction insertFunctionMapType = static_cast<InsertMapFunction>(&T::operator[]);
			UnorderedMapUnrefConstIteratorFunc unref = &MapConstIterator::operator->;
			IncrementMapIterator increment = &MapIterator::operator++;


			ReflectMapFunction reflectMapFunction =
			{
				.reserveFunction = *reinterpret_cast<uint64_t*>(&reserverFunctionMapType),
				.insertFunction = *reinterpret_cast<uint64_t*>(&insertFunctionMapType),
				.unrefFunc = *reinterpret_cast<uint64_t*>(&unref),
				.incrementFunc = *reinterpret_cast<uint64_t*>(&insertFunctionMapType),
			};

			typeMetaData->data = rm;
			m_UnordoredMapReflectFunction.insert({ GetTypeKey<T>(), reflectMapFunction });
		}

		if constexpr (is_sparse_set<T>::value)
		{
			ReflectedSparseSet sparsetReflected;
			
			// unsure that those type are reflected as well
			ReflectType<std::vector<typename T::DenseType>>();
			sparsetReflected.denseVector = GetTypeKey<std::vector<typename T::DenseType>>();

			ReflectType<std::vector<typename T::SparseType>>();
			sparsetReflected.spareVector = GetTypeKey<std::vector<typename T::SparseType>>();

			// HARDOCODED TO FIND A BETTER WAY
			sparsetReflected.spareVectorOffset = sizeof(std::vector<typename T::DenseType>);
			sparsetReflected.denseVectorOffSet = 0;

			typeMetaData->data = sparsetReflected;
		}
		

		if constexpr (is_bit_set<T>::value)
		{
			//typeMetaData->typeNatureMetaData = ReflectedBitSet(T::size); // TODO
			typeMetaData->data = ReflectedBitSet();
		}

		if constexpr (!std::is_abstract_v<T>)
		{
			typeMetaData->createFunc = &ReflectedCreateFunc<T>;
			typeMetaData->deleteFunc = &ReflectedDeleteFunc<T>;
		}
		
		if constexpr (std::is_invocable_v<std::hash<T>, const T&>)
		{
			typeMetaData->hashFun = (HashFunc)&HashFunction<T>;
		}
		else
		{
			typeMetaData->hashFun = nullptr;
		}

		if constexpr (std::is_same_v<T, std::filesystem::path>)
		{
			typeMetaData->data = ReflectedFileSystemPath(GetTypeKey<T>());
		}

		if constexpr (std::is_enum_v<T>)
		{
			ReflectedEnum reflectedEnum;

			// Iterate all enum values using magic_enum
			for (auto e : magic_enum::enum_values<T>()) {
				reflectedEnum.members.emplace_back(EnumMember{
					std::string(magic_enum::enum_name(e)),
					static_cast<uint8_t>(e)
					});
			}
			reflectedEnum.name = GetCorrectNameFromTypeId(typeid(T).name());
			typeMetaData->data = std::move(reflectedEnum);
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

template<typename T>
const ReflectedEnum& Reflector::GetEnum()
{
	static_assert(std::is_enum_v<T>);
	
	auto it = m_RelfectionMap.find(GetTypeKey<T>());
	if (it == m_RelfectionMap.end())
		AddType<T>();

	return std::get<ReflectedEnum>(m_RelfectionMap.at(GetTypeKey<T>()).metaData.data);
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
	const ReflectedType* currentType = &type;
	const ReflectedType& baseType = GetType<Base>();

	if (type == baseType)
		return false;

	while (currentType->metaData.baseClass != NullTypeId)
	{
		if (currentType->metaData.baseClass == baseType.typeId)
			return true;

		currentType = &GetType(currentType->metaData.baseClass);
	}

	return false;
}


template <typename Holder, typename MemberType, MemberEnumFlag memberEnumFlag>
uint8_t Reflector::ReflectMember(size_t _offset, const char* _memberName)
{
	PERF_REGION_SCOPED;
	
	std::unordered_map<uint32_t, ReflectedType>& memberMap = m_RelfectionMap;

	if (!ContaintType<Holder>())
	{
		PC_LOGERROR("ReflectMember Holder member not found")
			return 0;
	}

	if (!ContaintType<MemberType>())
	{
		AddType<MemberType>();
	}
	constexpr uint32_t holderKey = GetTypeKey<Holder>();
	for (const auto& member : memberMap.at(holderKey).metaData.members)
	{
		// is there aldready a member name as
		if (member.membersName == _memberName)
			return 0;

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
	return 0;
}

template <typename Holder, typename BaseClass>
uint8_t Reflector::ReflectType()
{
	PERF_REGION_SCOPED;

	constexpr uint32_t KeyHolder = GetTypeKey<Holder>();

	if (ContaintType<Holder>())
	{
		return 0;
	}


	AddType<Holder>();

	// Add base class to current class and all hieritance
	if constexpr (!std::is_same_v<void, BaseClass> || std::is_base_of_v<BaseClass, Holder>)
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
			);
		}

	}

	return 0;
}

template <typename T>
std::vector<const ReflectedType*> Reflector::GetAllTypesFrom()
{
	PERF_REGION_SCOPED;
	
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
	return isTrivialType(GetTypeKey<T>());
}

template <typename T, typename F>
bool Reflector::GetPtrToTypeField(T* _object, const std::string& _fieldName, F** _outPtrToField)
{
	return GetPtrToTypeField(GetTypeKey<T>(), _object, _fieldName, reinterpret_cast<void**>(_outPtrToField));
}

template <typename T, typename F>
bool Reflector::GetPtrToTypeField(const T& _object, const std::string& _fieldName, const F** _outPtrToField)
{
	return GetPtrToTypeField(GetTypeKey<T>(), &_object, _fieldName, reinterpret_cast<void**>(_outPtrToField));
}


template <typename T>
void Reflector::AddType()
{
	PERF_REGION_SCOPED;
	
	if (!ContaintType<T>())
	{
		// Create New Node in map
		std::string name = GetCorrectNameFromTypeId(typeid(T).name());

		TypeId typeId = GetTypeKey<T>();

		ReflectedType type{};
		type.typeId = typeId;
		type.typeFlags = {};
		type.name = name;
		type.size = sizeof(T);
		type.alignment = alignof(T);
		type.metaData = {};
		type.rttiTypeId = typeid(T).hash_code();
		

		type.typeFlags = ProcessMetaData<T>(&type.metaData);
		m_RttiToTypeId.insert({ type.rttiTypeId, typeId });
		m_RelfectionMap.insert({ typeId,type });
	}
}


template <typename T>
bool Reflector::ContaintType()
{
	return Containt(GetTypeKey<T>());
}

//https://isocpp.org/files/papers/P3384R0.html
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define NEW_VAR(name) CONCAT(name, __COUNTER__)

#define REFLECT(CurrentType, ...) \
static inline uint8_t CONCAT(reflectInfo,__COUNTER__) = PC_CORE::Reflector::ReflectType<CurrentType, ##__VA_ARGS__>();\

#define REFLECT_MEMBER(CurrentType, memberName, ...) \
static inline uint8_t CurrentType##_##memberName##_reflected = \
PC_CORE::Reflector::ReflectMember<CurrentType, decltype(CurrentType::memberName), ##__VA_ARGS__>( \
PC_CORE::offset_of(&CurrentType::memberName), #memberName);

END_PCCORE


