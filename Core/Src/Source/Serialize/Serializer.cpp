#include "serialize/serializer.h"


#include <fstream>
#include <any>
#include <unordered_map>
#include <variant>

#include "GLFW/glfw3native.h"
#include "Resources/Resource.hpp"
#include "Resources/ResourceManager.hpp"

using namespace PC_CORE;

// bool //
constexpr const char* boolAlpha0s = "false";
constexpr const char* boolAlpha1s = "true";
constexpr bool boolAlpha0b = false;
constexpr bool boolAlpha1b = true;
//

// unordoredMap Function
using UnordoredByteMap = std::unordered_map<uint8_t, uint8_t>;
using UnordoredMapIterator = typename UnordoredByteMap::iterator;
using IncrementMapIterator = UnordoredMapIterator & (UnordoredMapIterator::*)();
using ReseverMapFunction = void (UnordoredByteMap::*)(size_t);
using InsertMapFunction = typename UnordoredByteMap::mapped_type& (UnordoredByteMap::*)(const typename UnordoredByteMap::key_type&);

using UnordoredMapConstIterator = typename UnordoredByteMap::const_iterator;
using UnorderedMapUnrefConstIteratorFunc = const std::pair<const typename UnordoredByteMap::key_type, typename UnordoredByteMap::mapped_type>* (UnordoredMapConstIterator::*)() const;
//



#pragma region Serialization
void TypeToString(json& outj, TypeId id, const uint8_t* objetPtr)
{

	if (Reflector::IsTypeIdIs<bool>(id))
	{
		const bool* b = reinterpret_cast<const bool*>(objetPtr);
		outj = *b ? boolAlpha1s : boolAlpha0s;
	}
	else if (Reflector::IsTypeIdIs<char>(id))
	{
		const char* b = reinterpret_cast<const char*>(objetPtr);
		outj = *b;

	}
	else if (Reflector::IsTypeIdIs<uint8_t>(id))
	{
		const uint8_t* b = reinterpret_cast<const uint8_t*>(objetPtr);
		outj = *b;
	}
	else if (Reflector::IsTypeIdIs<uint16_t>(id))
	{
		const uint16_t* b = reinterpret_cast<const uint16_t*>(objetPtr);
		outj = *b;
	}
	else if (Reflector::IsTypeIdIs<int>(id))
	{
		const int* b = reinterpret_cast<const int*>(objetPtr);
		outj = *b;
	}
	else if (Reflector::IsTypeIdIs<uint32_t>(id))
	{
		const uint32_t* b = reinterpret_cast<const uint32_t*>(objetPtr);
		outj = *b;
	}
	else if (Reflector::IsTypeIdIs<uint64_t>(id))
	{
		const uint64_t* b = reinterpret_cast<const uint64_t*>(objetPtr);
		outj = *b;
	}
	else if (Reflector::IsTypeIdIs<float>(id))
	{
		const float* f = reinterpret_cast<const float*>(objetPtr);
		outj = *f;
	}
	else if (Reflector::IsTypeIdIs<double>(id))
	{
		const double* f = reinterpret_cast<const double*>(objetPtr);
		outj = *f;
	}
}


void SerializeMember(json& _jsonFile, const Members& member, const uint8_t* objetPtr);
void SerializeType(json& _jsonFile, const uint8_t* objetPtr, TypeId _typeKey);


void SerializeMember(json& _jsonFile, const Members& member, const uint8_t* objetPtr)
{
	if (member.memberFlag & MemberEnumFlag::SERIALIZE)
		return;

	auto& type = PC_CORE::Reflector::GetType(member.typeKey);
	json& memberJson = _jsonFile[type.name][member.membersName];

	SerializeType(memberJson, objetPtr, member.typeKey);

}



void SerializeType(json& _jsonFile, const uint8_t* objetPtr, TypeId _typeKey)
{
	const ReflectedType& type = Reflector::GetType(_typeKey);
	PERF_REGION_SCOPED_NAME_DYNAMIC(("SerializeType : " + type.name).c_str());
	if (!std::holds_alternative<std::monostate>(type.metaData.data))
	{
		std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, ReflectedWeakPtr>)
				{
					const uint64_t ptr = (uint64_t)objetPtr;
					const ReflectedWeakPtr& wkPtr = std::get<ReflectedWeakPtr>(type.metaData.data);
					const ReflectedType& typeRef = Reflector::GetType(wkPtr.type);

					if (Reflector::IsBaseOf<Resource>(typeRef))
					{
						const std::weak_ptr<PC_CORE::Resource>* doublePtr = reinterpret_cast<const std::weak_ptr<PC_CORE::Resource>*>(ptr);

						if (doublePtr->expired())
						{
							return;
						}
						std::shared_ptr<PC_CORE::Resource> resourceSPtr = doublePtr->lock();
						try
						{

							if (resourceSPtr != nullptr)
							{
								_jsonFile["Guid"] = std::string(resourceSPtr->GetGuid());
							}
							else
							{
								_jsonFile["Guid"] = Guid::Empty();
							}
						}
						catch (...)
						{
							return;
						}

					}
				}
				else if constexpr (std::is_same_v<T, ReflectedSharedPtr>)
				{
					const ReflectedSharedPtr& srdPtr = std::get<ReflectedSharedPtr>(type.metaData.data);
					const ReflectedType& pointedType = Reflector::GetType(srdPtr.type);

					const std::shared_ptr<Resource>* rsPtr = reinterpret_cast<const std::shared_ptr<Resource>*>(objetPtr);
					const Resource* rsInterfaceDummie = reinterpret_cast<const Resource*>(rsPtr->get());

					assert(rsInterfaceDummie->GetType().typeId != Reflector::GetTypeKey<Resource>());
					try
					{
						_jsonFile[Serializer::RESOURCE_TYPE] = rsInterfaceDummie->GetType().typeId;
					}
					catch (...)
					{
						return;
					}

					SerializeType(_jsonFile, reinterpret_cast<const uint8_t*>(rsPtr->get()), pointedType.typeId);
				}
				else if constexpr (std::is_same_v<T, ReflectedArray>)
				{
					const ReflectedArray& arr = std::get<ReflectedArray>(type.metaData.data);
					const ReflectedType& underLineType = Reflector::GetType(arr.type);
					try
					{
						_jsonFile[Serializer::CONTAINER_SIZE] = arr.size;

						if (Reflector::isTrivialType(underLineType.typeId))
						{
							std::vector<uint8_t> data;
							data.resize(arr.size * underLineType.size);
							std::memcpy(data.data(), objetPtr, data.size());
							_jsonFile[Serializer::DATA] = json::binary(data);
						}
						else
						{
							for (size_t i = 0; i < arr.size; i++)
							{
								const size_t offSet = i * underLineType.size;
								SerializeType(_jsonFile[std::to_string(i)], objetPtr + offSet, underLineType.typeId);
							}
						}


					}
					catch (...)
					{
						return;
					}
				}
				else if constexpr (std::is_same_v<T, ReflectedVector>)
				{
					const ReflectedVector& rVector = std::get<ReflectedVector>(type.metaData.data);
					const ReflectedType& underLineType = Reflector::GetType(rVector.type);
					const std::vector<uint8_t>* ver = reinterpret_cast<const std::vector<uint8_t>*>(objetPtr);
					const size_t typeCount = ver->size() / underLineType.size;

					try
					{
						_jsonFile[Serializer::CONTAINER_SIZE] = typeCount;
					}
					catch (...)
					{
						return;
					}

					if (Reflector::isTrivialType(underLineType.typeId))
					{

						_jsonFile[Serializer::DATA] = json::binary(*ver);

					}
					else
					{
						for (size_t i = 0; i < typeCount; i++)
						{
							const size_t offSet = i * underLineType.size;
							SerializeType(_jsonFile[std::to_string(i)], ver->data() + offSet, underLineType.typeId);
						}
					}
				}
				else if constexpr (std::is_same_v<T, ReflectedString>)
				{
					const ReflectedString& rs = std::get<ReflectedString>(type.metaData.data);
					try
					{
						if (rs.subType == Reflector::GetTypeKey<char>())
						{
							const std::string* s = reinterpret_cast<const std::string*>(objetPtr);
							_jsonFile[Serializer::CONTAINER_SIZE] = s->size();
							_jsonFile["string"] = s->c_str();
						}
						else if (rs.subType == Reflector::GetTypeKey<wchar_t>())
						{
							const std::wstring* s = reinterpret_cast<const std::wstring*>(objetPtr);
							_jsonFile[Serializer::CONTAINER_SIZE] = s->size();
							_jsonFile["string"] = s->c_str();
						}
						else
						{
							assert(false);
						}
					}
					catch (...)
					{
						return;
					}

				}
				else if constexpr (std::is_same_v<T, ReflectedMap>)
				{
					const ReflectedMap& reflectedMap = std::get<ReflectedMap>(type.metaData.data);
					uint8_t* dirtyPtr = const_cast<uint8_t*>(objetPtr);
					UnordoredByteMap* map = reinterpret_cast<UnordoredByteMap*>(dirtyPtr);

					const ReflectedType& keyType = Reflector::GetType(reflectedMap.key);
					const ReflectedType& valueType = Reflector::GetType(reflectedMap.value);
					const ReflectMapFunction& reflectMapFunction = Reflector::m_UnordoredMapReflectFunction.at(type.typeId);


					const size_t mapSize = map->size();
					try
					{
						_jsonFile[Serializer::CONTAINER_SIZE] = mapSize;
					}
					catch (...)
					{
						return;
					}
					std::string indexs;

					UnorderedMapUnrefConstIteratorFunc unrefFunf = nullptr;
					std::memcpy(&unrefFunf, &reflectMapFunction.unrefFunc, sizeof(UnorderedMapUnrefConstIteratorFunc));

					auto mapBegin = map->begin();


					for (size_t i = 0; i < mapSize; i++)
					{
						indexs = std::to_string(i);

						auto* pair = (mapBegin.*unrefFunf)();
						try
						{
							const uint8_t* keyPtr = reinterpret_cast<const uint8_t*>(pair);
							const uint8_t* valuePtr = keyPtr + reflectedMap.offsetBetweenKeyAndValueInPair;

							SerializeType(_jsonFile[indexs][Serializer::KEY], keyPtr, keyType.typeId);
							SerializeType(_jsonFile[indexs][Serializer::VALUE], valuePtr, valueType.typeId);
						}
						catch (...)
						{

						}

						mapBegin++;
					}

				}
				else if constexpr (std::is_same_v<T, ReflectedBitSet>)
				{
					_jsonFile[Serializer::CONTAINER_SIZE] = type.size;
					std::vector<uint8_t> data(type.size);
					std::memcpy(data.data(), objetPtr, data.size());
					_jsonFile[Serializer::DATA] = json::binary(data);
				}
				else if constexpr (std::is_same_v<T, ReflectedSparseSet>)
				{
					const ReflectedSparseSet& reflectedSparSet = std::get<ReflectedSparseSet>(type.metaData.data);

					// dense vector
					SerializeType(_jsonFile[Serializer::SPARSE_SET_DENSE], objetPtr + reflectedSparSet.denseVectorOffSet, reflectedSparSet.denseVector);
					// spares vector
					SerializeType(_jsonFile[Serializer::SPARSE_SET_SPARSE], objetPtr + reflectedSparSet.spareVectorOffset, reflectedSparSet.spareVector);

				}
				else if constexpr (std::is_same_v<T, ReflectedFileSystemPath>)
				{
					const std::filesystem::path& reflectedSparSet = *reinterpret_cast<const std::filesystem::path*>(objetPtr);
					const auto s = reflectedSparSet.generic_string();

					_jsonFile[Serializer::CONTAINER_SIZE] = s.size();
					_jsonFile["string"] = s.c_str();
				}
				else if constexpr (std::is_same_v<T, ReflectedEnum>)
				{
					// TODO
					const ReflectedEnum& renum = std::get<ReflectedEnum>(type.metaData.data);
					uint8_t v = *objetPtr;
					bool hasFoundValue = false;
					const std::string* name = nullptr;
					for (auto& it : renum.members)
					{
						if (it.value == v)
						{
							name = &it.name;
							hasFoundValue = true;
							break;
						}
					}
					assert(hasFoundValue);

					_jsonFile = name->c_str();
				}

			}, type.metaData.data);
		return;
	}


	if (type.typeFlags & TypeFlagBits::COMPOSITE)
	{
		for (auto& member : type.metaData.members)
		{
			if (member.memberFlag & MemberEnumFlag::SERIALIZE)
				continue;

			const uint8_t* ptr = objetPtr + member.offset;

			SerializeMember(_jsonFile, member, ptr);
		}
	}
	else
	{
		TypeToString(_jsonFile, _typeKey, objetPtr);
	}
}



void PC_CORE::Serializer::Serializing(const uint8_t* objetPtr, TypeId _typeKey, const std::string& _fileToSerialize)
{
	std::ofstream myfile(_fileToSerialize);

	if (!myfile.is_open())
	{
		return;
	}
	const ReflectedType& type = Reflector::GetType(_typeKey);

	json j;
	SerializeType(j[type.name], objetPtr, _typeKey);

	{
		PERF_REGION_SCOPED_NAMED("Dump JSON");
		myfile << std::setw(4) << j.dump(4);
	}

	myfile.close();
}
#pragma endregion


#pragma region DeSerialization

void TypeFromString(const json& json, TypeId id, uint8_t* objetPtr)
{

	if (Reflector::IsTypeIdIs<bool>(id))
	{
		bool* b = reinterpret_cast<bool*>(objetPtr);
		std::string get = json.template get<std::string>();

		if (strcmp("true", get.c_str()) == 0) *b = true;
		else if (strcmp("false", get.c_str()) == 0) *b = false;
		else assert(false);;



	}
	else if (Reflector::IsTypeIdIs<char>(id))
	{
		char* b = reinterpret_cast<char*>(objetPtr);
		char get = json.template get<char>();

		*b = get;

	}
	else if (Reflector::IsTypeIdIs<uint8_t>(id))
	{
		uint8_t* b = reinterpret_cast<uint8_t*>(objetPtr);
		uint8_t get = json.template get<uint8_t>();

		*b = get;
	}
	else if (Reflector::IsTypeIdIs<uint16_t>(id))
	{
		uint16_t* b = reinterpret_cast<uint16_t*>(objetPtr);
		uint16_t get = json.template get<uint16_t>();

		*b = get;
	}
	else if (Reflector::IsTypeIdIs<int>(id))
	{
		int* b = reinterpret_cast<int*>(objetPtr);
		int get = json.template get<int>();

		*b = get;
	}
	else if (Reflector::IsTypeIdIs<uint32_t>(id))
	{
		uint32_t* b = reinterpret_cast<uint32_t*>(objetPtr);
		uint32_t get = json.template get<uint32_t>();

		*b = get;
	}
	else if (Reflector::IsTypeIdIs<uint64_t>(id))
	{
		uint64_t* b = reinterpret_cast<uint64_t*>(objetPtr);
		uint64_t get = json.template get<uint64_t>();

		*b = get;
	}
	else if (Reflector::IsTypeIdIs<float>(id))
	{
		float* f = reinterpret_cast<float*>(objetPtr);
		float get = json.template get<float>();

		*f = get;
	}
	else if (Reflector::IsTypeIdIs<double>(id))
	{
		double* f = reinterpret_cast<double*>(objetPtr);
		double get = json.template get<double>();

		*f = get;
	}
}

void DeSerializeMember(const json& _jsonFile, const Members& member, uint8_t* objetPtr);
void DeserializeType(const json& _jsonFile, uint8_t* objetPtr, TypeId _typeKey);


void DeSerializeMember(const json& _jsonFile, const Members& member, uint8_t* objetPtr)
{
	auto& type = PC_CORE::Reflector::GetType(member.typeKey);
	try
	{
		const json& memberJson = _jsonFile[type.name][member.membersName];
		DeserializeType(memberJson, objetPtr, member.typeKey);

		return;
	}
	catch (...) {}

}


void DeserializeType(const json& _jsonFile, uint8_t* objetPtr, TypeId _typeKey)
{
	const ReflectedType& type = Reflector::GetType(_typeKey);
	PERF_REGION_SCOPED_NAME_DYNAMIC(("DeserializeType : " + type.name).c_str());

	if (!std::holds_alternative<std::monostate>(type.metaData.data))
	{
		std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, ReflectedWeakPtr>)
				{
					const ReflectedWeakPtr& wkPtrR = std::get<ReflectedWeakPtr>(type.metaData.data);
					uint64_t ptr = (uint64_t)objetPtr;
					const auto& typeRef = Reflector::GetType(wkPtrR.type);

					if (Reflector::IsBaseOf<Resource>(typeRef))
					{
						ResourceRef<PC_CORE::Resource>* doublePtr = reinterpret_cast<ResourceRef<PC_CORE::Resource>*>(ptr);
						try
						{
							Guid g = Guid::FromString(_jsonFile["Guid"].get < std::string >().c_str());

							if (g == Guid::Empty())
							{
								doublePtr->reset();
							}
							else
							{
								std::shared_ptr<Resource> r = PC_CORE::ResourceManager::Get<Resource>(g);
								if (r == nullptr)
								{
									PC_LOGERROR("There is no matching guiid ");
								}
								*doublePtr = r;
							}
						}
						catch (...)
						{

						}
					}
				}
				else if constexpr (std::is_same_v<T, ReflectedSharedPtr>)
				{
					const ReflectedSharedPtr& shPtrR = std::get<ReflectedSharedPtr>(type.metaData.data);
					const auto& pointedType = Reflector::GetType(shPtrR.type);

					try
					{
						TypeId pointedTypeId = _jsonFile[Serializer::RESOURCE_TYPE];
						auto& pointedTypeInFile = Reflector::GetType(pointedTypeId);

						if (pointedTypeId == Reflector::GetTypeKey<Resource>() || Reflector::IsBaseOf<Resource>(pointedTypeInFile))
						{

							std::shared_ptr<Resource>* rsPtr = reinterpret_cast<std::shared_ptr<Resource>*>(objetPtr);


							const DeleteFunc deletFunc = pointedTypeInFile.metaData.deleteFunc;
							std::shared_ptr<uint8_t[]> byteBuffer(new uint8_t[pointedTypeInFile.size], [deletFunc](uint8_t* ptr)
								{
									deletFunc((void*)ptr);
								});
							*rsPtr = std::reinterpret_pointer_cast<Resource>(byteBuffer);

							assert(byteBuffer.use_count() == rsPtr->use_count());
							DeserializeType(_jsonFile, reinterpret_cast<uint8_t*>(rsPtr->get()), pointedTypeInFile.typeId);
							assert(byteBuffer.use_count() != 0 && rsPtr->use_count() != 0);


						}
					}
					catch (...)
					{

					}
				}
				else if constexpr (std::is_same_v<T, ReflectedArray>)
				{
					const ReflectedArray& arr = std::get<ReflectedArray>(type.metaData.data);
					const ReflectedType& underLineType = Reflector::GetType(arr.type);

					try
					{
						size_t s = _jsonFile[Serializer::CONTAINER_SIZE];
						if (_jsonFile[Serializer::CONTAINER_SIZE] != arr.size)
						{
							PC_LOGERROR("array size missmacht")
								return;
						}
						if (Reflector::isTrivialType(underLineType.typeId))
						{
							std::vector<uint8_t> bytes = _jsonFile["data"]["bytes"].get<std::vector<uint8_t>>();
							std::memcpy(objetPtr, bytes.data(), arr.size * underLineType.size);
						}
						else
						{
							for (size_t i = 0; i < arr.size; i++)
							{
								const size_t offSet = i * underLineType.size;
								DeserializeType(_jsonFile[std::to_string(i)], objetPtr + offSet, underLineType.typeId);
							}
						}

					}
					catch (const std::exception& e)
					{
						std::cerr << "Exception caught: " << e.what() << std::endl;

					}
				}
				else if constexpr (std::is_same_v<T, ReflectedVector>)
				{
					std::vector<uint8_t>* ver = reinterpret_cast<std::vector<uint8_t>*>(objetPtr);
					const ReflectedVector& vector = std::get<ReflectedVector>(type.metaData.data);
					const ReflectedType& underLineType = Reflector::GetType(vector.type);
					try
					{

						if (Reflector::isTrivialType(underLineType.typeId))
						{
							*ver = _jsonFile[Serializer::DATA]["bytes"].get<std::vector<uint8_t>>();
						}
						else
						{
							const size_t size = _jsonFile[Serializer::CONTAINER_SIZE];
							ver->resize(size * underLineType.size);
							for (size_t i = 0; i < size; i++)
							{
								const size_t offSet = i * underLineType.size;
								DeserializeType(_jsonFile[std::to_string(i)], ver->data() + offSet, underLineType.typeId);
							}
						}
					}
					catch (...)
					{

					}
				}
				else if constexpr (std::is_same_v<T, ReflectedString>)
				{
					const ReflectedString& rs = std::get<ReflectedString>(type.metaData.data);
					try
					{
						if (rs.subType == Reflector::GetTypeKey<char>())
						{
							std::string* s = reinterpret_cast<std::string*>(objetPtr);
							s->resize(_jsonFile[Serializer::CONTAINER_SIZE]);
							std::string_view v = _jsonFile["string"];

							memcpy(s->data(), v.data(), s->size());
						}
						else if (rs.subType == Reflector::GetTypeKey<wchar_t>())
						{
							std::wstring* s = reinterpret_cast<std::wstring*>(objetPtr);
							s->resize(_jsonFile[Serializer::CONTAINER_SIZE]);
							*s = _jsonFile["string"].template get<typename std::wstring>();
						}
						else
						{
							assert(false);
						}
					}
					catch (...)
					{

					}

				}
				else if constexpr (std::is_same_v<T, ReflectedMap>)
				{
					UnordoredByteMap& map = *reinterpret_cast<UnordoredByteMap*>(objetPtr);
					const ReflectedMap& reflectedMap = std::get<ReflectedMap>(type.metaData.data);

					const ReflectedType& keyType = Reflector::GetType(reflectedMap.key);
					const ReflectedType& valueType = Reflector::GetType(reflectedMap.value);
					const ReflectMapFunction& reflectMapFunction = Reflector::m_UnordoredMapReflectFunction.at(type.typeId);

					ReseverMapFunction rfunc = nullptr;
					std::memcpy(&rfunc, &reflectMapFunction.reserveFunction, sizeof(ReseverMapFunction));
					InsertMapFunction inserFunc = nullptr;
					std::memcpy(&inserFunc, &reflectMapFunction.insertFunction, sizeof(InsertMapFunction));
					std::unique_ptr<uint8_t[]> keyBuffer = std::make_unique<uint8_t[]>(keyType.size);

					try
					{
						const size_t size = _jsonFile[Serializer::CONTAINER_SIZE];

						std::string indexs;
						for (size_t i = 0; i < size; i++)
						{
							indexs = std::to_string(i);

							try
							{
								DeserializeType(_jsonFile[indexs][Serializer::KEY], keyBuffer.get(), keyType.typeId);
								auto* ref = &(map.*inserFunc)(*keyBuffer.get());

								DeserializeType(_jsonFile[indexs][Serializer::VALUE], ref, valueType.typeId);
							}
							catch (...)
							{
								continue;
							}

						}
					}
					catch (...)
					{

					}
				}
				else if constexpr (std::is_same_v<T, ReflectedBitSet>)
				{
					const size_t s = _jsonFile[Serializer::CONTAINER_SIZE];
					if (_jsonFile[Serializer::CONTAINER_SIZE] != type.size)
					{
						PC_LOGERROR("bitset size missmacht")
					}

					std::vector<uint8_t> bytes = _jsonFile["data"]["bytes"].get<std::vector<uint8_t>>();
					std::memcpy(objetPtr, bytes.data(), type.size);
					return;
				}
				else if constexpr (std::is_same_v<T, ReflectedSparseSet>)
				{
					const ReflectedSparseSet& reflectedSparSet = std::get<ReflectedSparseSet>(type.metaData.data);

					// dense vector
					DeserializeType(_jsonFile[Serializer::SPARSE_SET_DENSE], objetPtr + reflectedSparSet.denseVectorOffSet, reflectedSparSet.denseVector);

					// spares vector
					DeserializeType(_jsonFile[Serializer::SPARSE_SET_SPARSE], objetPtr + reflectedSparSet.spareVectorOffset, reflectedSparSet.spareVector);
				}
				else if constexpr (std::is_same_v<T, ReflectedFileSystemPath>)
				{
					const std::filesystem::path& reflectedSparSet = *reinterpret_cast<const std::filesystem::path*>(objetPtr);
					std::string s;
					s.resize(_jsonFile[Serializer::CONTAINER_SIZE]);
					std::string_view v = _jsonFile["string"];

					memcpy(s.data(), v.data(), s.size());

					std::filesystem::path* p = reinterpret_cast<std::filesystem::path*>(objetPtr);
					*p = std::filesystem::path(std::move(s));
				}
				else if constexpr (std::is_same_v<T, ReflectedEnum>)
				{
					//  TODO
					const ReflectedEnum& renum = std::get<ReflectedEnum>(type.metaData.data);
					std::string_view v = _jsonFile; // get the enum as string
					bool hasFoundValue = false;

					for (auto& it : renum.members)
					{
						if (it.name == v)
						{
							hasFoundValue = true;
							*objetPtr = it.value;
							break;
						}
					}
					assert(hasFoundValue);
				}
				else if constexpr (std::is_same_v<T, std::monostate>)
				{

				}

			}, type.metaData.data);
		return;
	}


	if (type.typeFlags & TypeFlagBits::COMPOSITE)
	{
		if (type.metaData.createFunc != nullptr)
			type.metaData.createFunc(objetPtr);

		for (auto& member : type.metaData.members)
		{
			if (member.memberFlag & MemberEnumFlag::SERIALIZE)
				continue;

			uint8_t* ptr = objetPtr + member.offset;

			DeSerializeMember(_jsonFile, member, ptr);
		}
	}
	else
	{
		TypeFromString(_jsonFile, _typeKey, objetPtr);
	}

}


void Serializer::Derializing(uint8_t* _objetPtr, TypeId _typeKey, const std::string& _fileToSerialize)
{
	PERF_REGION_SCOPED;

	json j;
	std::ifstream f(_fileToSerialize);
	if (!f.is_open())
	{
		f.close();
		PC_LOGERROR("Failed to open file {}", _fileToSerialize);
		return;
	}

	{

		PERF_REGION_SCOPED_NAMED("Parse JSON");
		j = json::parse(f);
		f.close();
	}


	const ReflectedType& type = Reflector::GetType(_typeKey);
	DeserializeType(j[type.name], _objetPtr, _typeKey);
}

#pragma endregion
