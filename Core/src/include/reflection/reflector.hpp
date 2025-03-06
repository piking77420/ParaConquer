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


	PC_CORE_API static inline std::unordered_map<TypeId, ReflectMapFunction> m_MapReflectFunction;

	PC_CORE_API static inline std::unordered_map<TypeId, ReflectMapFunction> m_UnordoredMapReflectFunction;
private:

	constexpr PC_CORE_API  static std::string GetCorrectNameFromTypeId(const std::string& _name)
	{
		// Remove the nameSpace
		const size_t firstIndex = _name.find_first_of("::", 0);
		std::string out;

		if (firstIndex != std::numeric_limits<size_t>::max())
		{
			for (size_t i = firstIndex + 2; i < _name.size(); i++)
				out.push_back(_name[i]);
		}
		else
		{
			const size_t secondIndex = _name.find_first_of(" ", 0);
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
		if constexpr (is_weak_ptr_v<T>)
		{
			typeMetaData->typeNatureMetaData.metaDataTypeEnum = TypeNatureMetaDataEnum::ResourceRefType;
			ResourceRefType& ptrtype = typeMetaData->typeNatureMetaData.metaDataType.resourceRef;

			TypeId ptrType = GetTypeKey<typename T::element_type>();
			if (!m_RelfectionMap.contains(ptrType))
			{
				PC_LOGERROR("Try to reflect ptr without reflect the type before");
			}
			ptrtype.type = ptrType;

		}
		if constexpr (is_shared_ptr_v<T>)
		{
			typeMetaData->typeNatureMetaData.metaDataTypeEnum = TypeNatureMetaDataEnum::ResourceRefType;
			ResourceRefType& ptrtype = typeMetaData->typeNatureMetaData.metaDataType.resourceRef;

			TypeId ptrType = GetTypeKey<typename T::element_type>();
			if (!m_RelfectionMap.contains(ptrType))
			{
				PC_LOGERROR("Try to reflect ptr without reflect the type before");
			}
			ptrtype.type = ptrType;

		}

		if constexpr (is_std_array_v<T>)
		{
			typeMetaData->typeNatureMetaData.metaDataTypeEnum = TypeNatureMetaDataEnum::Array;
			Array& arrayType = typeMetaData->typeNatureMetaData.metaDataType.array;
			arrayType.type = GetTypeKey<typename T::value_type>();
			arrayType.size  = std::tuple_size_v<T>;
		}


		if constexpr (std::is_array_v<T>)
		{
			typeMetaData->typeNatureMetaData.metaDataTypeEnum = TypeNatureMetaDataEnum::Array;
			Array& arrayType = typeMetaData->typeNatureMetaData.metaDataType.array;
			arrayType.type = GetTypeKey<std::remove_extent_t<T>>();
			// Determine the size of the array
			constexpr std::size_t arraySize = std::extent_v<T>;
			arrayType.size = arraySize;
		}
		if constexpr (is_vector_v<T>)
		{
			typeMetaData->typeNatureMetaData.metaDataTypeEnum = TypeNatureMetaDataEnum::Vector;
			Vector& vectorType = typeMetaData->typeNatureMetaData.metaDataType.vector;
			vectorType.type = GetTypeKey<typename std::remove_extent<typename T::value_type>::type>();

		}
		if constexpr (std::is_same_v<std::string, T> || std::is_same_v<std::wstring, T>)
		{
			bool constexpr isw = std::is_same_v<std::wstring, T>;

			typeMetaData->typeNatureMetaData.metaDataTypeEnum = TypeNatureMetaDataEnum::String;
			RelfectedString& relfectedString = typeMetaData->typeNatureMetaData.metaDataType.relfectedString;
			relfectedString.type = isw ? GetTypeKey<wchar_t>() : GetTypeKey<char>();
		}


		if constexpr (is_map<T>::value)
		{
			typeMetaData->typeNatureMetaData.metaDataTypeEnum = TypeNatureMetaDataEnum::Map;
			ReflectedMap& rm = typeMetaData->typeNatureMetaData.metaDataType.mapReflected;

			using MapIterator = typename T::iterator;
			using UnrefIteratorFuncType = std::pair<const typename T::key_type, typename T::mapped_type>* (MapIterator::*)() const;
			using IncrementMapIterator = MapIterator & (MapIterator::*)();
			using ReserverFunctionMap = void (T::*)(size_t);
			using InsertFunctionMap = std::pair<typename T::iterator, bool>(T::*)(const typename T::value_type&);


			IncrementMapIterator incrementIteratorFuncType = &MapIterator::operator++;
			UnrefIteratorFuncType unrefIteratorFuncType = &MapIterator::operator->;

			uint64_t incrementIteratorFunc = *reinterpret_cast<uint64_t*>(&incrementIteratorFuncType);
			uint64_t unrefIteratorFunc = *reinterpret_cast<uint64_t*>(&unrefIteratorFuncType);

			ReflectMapFunction reflectMapFunction =
			{
				.incrementIterator = incrementIteratorFunc,
				.unrefIterator = unrefIteratorFunc,
			};

			m_MapReflectFunction.insert(GetTypeKey<T>(), reflectMapFunction);

		}

		if constexpr (is_unordered_map<T>::value)
		{
			typeMetaData->typeNatureMetaData.metaDataTypeEnum = TypeNatureMetaDataEnum::UnordoredMap;
			ReflectedMap& rm = typeMetaData->typeNatureMetaData.metaDataType.unordoredMapReflected;
			rm.key = GetTypeKey<typename T::key_type>();
			rm.value = GetTypeKey<typename T::mapped_type>();

			using MapIterator = typename T::iterator;
			using UnrefMapIteratorFuncType = std::pair<const typename T::key_type, typename T::mapped_type>* (MapIterator::*)() const;
			using IncrementMapIterator = MapIterator & (MapIterator::*)();
			using ReseverMapFunction = void (T::*)(size_t);
			//using InsertMapFunction = std::pair<typename T::iterator, bool>(T::*)(const typename T::value_type&);
			using InsertMapFunction = typename T::mapped_type& (T::*)(const typename T::key_type&);

			IncrementMapIterator incrementIteratorFuncType = &MapIterator::operator++;
			UnrefMapIteratorFuncType unrefIteratorFuncType = &MapIterator::operator->;
			ReseverMapFunction reserverFunctionMapType = &T::reserve;
			//InsertMapFunction insertFunctionMapType = static_cast<InsertMapFunction>(&T::insert);
			InsertMapFunction insertFunctionMapType = static_cast<InsertMapFunction>(&T::operator[]);


			ReflectMapFunction reflectMapFunction =
			{
				.incrementIterator = *reinterpret_cast<uint64_t*>(&incrementIteratorFuncType),
				.unrefIterator = *reinterpret_cast<uint64_t*>(&unrefIteratorFuncType),
				.reserveFunction = *reinterpret_cast<uint64_t*>(&reserverFunctionMapType),
				.insertFunction = *reinterpret_cast<uint64_t*>(&insertFunctionMapType),
			};

			m_UnordoredMapReflectFunction.insert({ GetTypeKey<T>(), reflectMapFunction });
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
	for (auto&& member : memberMap.at(holderKey).metaData.members)
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


	uint32_t KeyHolder = GetTypeKey<Holder>();

	if (ContaintType<Holder>())
	{
		return &m_RelfectionMap.at(KeyHolder);
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
		m_RelfectionMap.insert({ typeId,type });
	}
}


template <typename T>
bool Reflector::ContaintType()
{
	return ContaintTypeFromTypeID(GetTypeKey<T>());
}


/*
template <class Tag>
struct stowed
{
	static typename Tag::type value;
};
template <class Tag>
typename Tag::type stowed<Tag>::value;

// Generate a static data member whose constructor initializes
// stowed<Tag>::value.  This type will only be named in an explicit
// instantiation, where it is legal to pass the address of a private
// member.
template <class Tag, typename Tag::type privateField>
struct stow_private
{
	stow_private() { stowed<Tag>::value = privateField; }
	static stow_private instance;
};
template <class Tag, typename Tag::type privateField>
stow_private<Tag, privateField> stow_private<Tag, privateField>::instance;


#define GetPrivateField(CurrentType, memberName)
struct A_x { typedef char const* (A::* type); };\
template class stow_private<A_x, &A::x>;\
*/

//https://isocpp.org/files/papers/P3384R0.html
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define NEW_VAR(name) CONCAT(name, __COUNTER__)

#define REFLECT(CurrentType, ...) \
static inline PC_CORE::ReflectedType* CONCAT(reflectInfo,__COUNTER__) = PC_CORE::Reflector::ReflectType<CurrentType, ##__VA_ARGS__>();\




#define REFLECT_MEMBER(CurrentType, memberName, ...) \
inline PC_CORE::Members CurrentType##_##memberName##_reflected = PC_CORE::Reflector::ReflectMember<CurrentType, decltype(CurrentType::memberName),##__VA_ARGS__>(offsetof(CurrentType, memberName), #memberName);\


#define GetPrivateField



END_PCCORE


