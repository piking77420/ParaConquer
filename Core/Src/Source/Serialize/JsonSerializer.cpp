#include "Serialize/JsonSerializer.hpp"

#include <fstream>
#include <any>
#include <unordered_map>
#include <variant>

using namespace PC_CORE;


void JsonSerializer::SerializeTrivial(PC_CORE::TypeId id, const uint8_t* objetPtr)
{
	if (Reflector::IsTypeIdIs<bool>(id))
	{
		const bool* b = reinterpret_cast<const bool*>(objetPtr);
		GetLastJson() = *b ? boolAlpha1s : boolAlpha0s;
	}
	else if (Reflector::IsTypeIdIs<char>(id))
	{
		const char* b = reinterpret_cast<const char*>(objetPtr);
		GetLastJson() = *b;

	}
	else if (Reflector::IsTypeIdIs<uint8_t>(id))
	{
		const uint8_t* b = reinterpret_cast<const uint8_t*>(objetPtr);
		GetLastJson() = *b;
	}
	else if (Reflector::IsTypeIdIs<uint16_t>(id))
	{
		const uint16_t* b = reinterpret_cast<const uint16_t*>(objetPtr);
		GetLastJson() = *b;
	}
	else if (Reflector::IsTypeIdIs<int>(id))
	{
		const int* b = reinterpret_cast<const int*>(objetPtr);
		GetLastJson() = *b;
	}
	else if (Reflector::IsTypeIdIs<uint32_t>(id))
	{
		const uint32_t* b = reinterpret_cast<const uint32_t*>(objetPtr);
		GetLastJson() = *b;
	}
	else if (Reflector::IsTypeIdIs<uint64_t>(id))
	{
		const uint64_t* b = reinterpret_cast<const uint64_t*>(objetPtr);
		GetLastJson() = *b;
	}
	else if (Reflector::IsTypeIdIs<float>(id))
	{
		const float* f = reinterpret_cast<const float*>(objetPtr);
		GetLastJson() = *f;
	}
	else if (Reflector::IsTypeIdIs<double>(id))
	{
		const double* f = reinterpret_cast<const double*>(objetPtr);
		GetLastJson() = *f;
	}
	else
	{
		assert(false);
	}

}

void JsonSerializer::DeSerializeTrivial(PC_CORE::TypeId id, uint8_t* objetPtr)
{
	PERF_REGION_SCOPED

	if (Reflector::IsTypeIdIs<bool>(id))
	{
		bool* b = reinterpret_cast<bool*>(objetPtr);
		std::string get = GetLastJson().template get<std::string>();

		if (strcmp("true", get.c_str()) == 0) *b = true;
		else if (strcmp("false", get.c_str()) == 0) *b = false;
		else assert(false);;

	}
	else if (Reflector::IsTypeIdIs<char>(id))
	{
		char* b = reinterpret_cast<char*>(objetPtr);
		char get = GetLastJson().template get<char>();

		*b = get;
	}
	else if (Reflector::IsTypeIdIs<uint8_t>(id))
	{
		uint8_t* b = reinterpret_cast<uint8_t*>(objetPtr);
		uint8_t get = GetLastJson().template get<uint8_t>();

		*b = get;
	}
	else if (Reflector::IsTypeIdIs<uint16_t>(id))
	{
		uint16_t* b = reinterpret_cast<uint16_t*>(objetPtr);
		uint16_t get = GetLastJson().template get<uint16_t>();

		*b = get;
	}
	else if (Reflector::IsTypeIdIs<int>(id))
	{
		int* b = reinterpret_cast<int*>(objetPtr);
		int get = GetLastJson().template get<int>();

		*b = get;
	}
	else if (Reflector::IsTypeIdIs<uint32_t>(id))
	{
		uint32_t* b = reinterpret_cast<uint32_t*>(objetPtr);
		uint32_t get = GetLastJson().template get<uint32_t>();

		*b = get;
	}
	else if (Reflector::IsTypeIdIs<uint64_t>(id))
	{
		uint64_t* b = reinterpret_cast<uint64_t*>(objetPtr);
		uint64_t get = GetLastJson().template get<uint64_t>();

		*b = get;
	}
	else if (Reflector::IsTypeIdIs<float>(id))
	{
		float* f = reinterpret_cast<float*>(objetPtr);
		float get = GetLastJson().template get<float>();

		*f = get;
	}
	else if (Reflector::IsTypeIdIs<double>(id))
	{
		double* f = reinterpret_cast<double*>(objetPtr);
		double get = GetLastJson().template get<double>();

		*f = get;
	}
}

void JsonSerializer::Serializing(const uint8_t* objetPtr, TypeId _typeKey)
{
	PERF_REGION_SCOPED
	const ReflectedType& type = Reflector::GetType(_typeKey);
	//Entry
	m_JsonStack.push_back(&m_MainJson[type.name]);
	SerializeType(objetPtr, _typeKey);
	m_JsonStack.pop_back();

	assert(m_JsonStack.empty());
}

void JsonSerializer::DeSerializing(uint8_t* _objetPtr, TypeId _typeKey)
{
	PERF_REGION_SCOPED

	const ReflectedType& type = Reflector::GetType(_typeKey);
	//Entry
	m_JsonStack.push_back(&m_MainJson[type.name]);
	DeserializeType(_objetPtr, _typeKey);
	m_JsonStack.pop_back();

	assert(m_JsonStack.empty());
}

void JsonSerializer::OpenFileForRead(const std::string& _fileToSerialize)
{
	PERF_REGION_SCOPED
	m_MainJson = {};

	m_Instream = std::ifstream(_fileToSerialize);

	if (!m_Instream.is_open())
	{
		m_Instream.close();
		PC_LOGERROR("Failed to open file {}", _fileToSerialize);
		return;
	}

	{
		PERF_REGION_SCOPED_NAMED("Parse JSON");
		m_MainJson = json::parse(m_Instream);
	}
}

void JsonSerializer::CloseForRead(const std::string& _fileToSerialize)
{
	PERF_REGION_SCOPED
	m_Instream.close();
	m_Instream = {};
}

void JsonSerializer::OpenFileForWrite(const std::string& _fileToSerialize)
{
	PERF_REGION_SCOPED;
	m_MainJson = {};

	m_OutStream = std::ofstream(_fileToSerialize);

	if (!m_OutStream.is_open())
	{
		PC_LOG("Failed to open file {}", _fileToSerialize);
		return;
	}
}

void JsonSerializer::CloseForWrite(const std::string& _fileToSerialize)
{
	PERF_REGION_SCOPED;
	{
		PERF_REGION_SCOPED_NAMED("Dump JSON");
		m_OutStream << std::setw(4) << m_MainJson.dump(4);
	}

	m_OutStream.close();
	m_OutStream = {};
}



void JsonSerializer::SerializeMember(const PC_CORE::Members& member, const uint8_t* objetPtr)
{
	PERF_REGION_SCOPED

	if (member.memberFlag & MemberEnumFlag::SERIALIZE)
		return;

	auto& type = PC_CORE::Reflector::GetType(member.typeKey);
	m_JsonStack.push_back(&GetLastJson()[type.name][member.membersName]);
	SerializeType(objetPtr, member.typeKey);
	m_JsonStack.pop_back();
}

void JsonSerializer::SerializeType(const uint8_t* objetPtr, PC_CORE::TypeId _typeKey)
{
	const ReflectedType& type = Reflector::GetType(_typeKey);
	PERF_REGION_SCOPED_NAME_DYNAMIC(("SerializeType : " + type.name).c_str());

	if (!std::holds_alternative<std::monostate>(type.metaData.data))
	{
		std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				
				if constexpr (std::is_same_v<T, ReflectedArray>)
				{
					const ReflectedArray& arr = std::get<ReflectedArray>(type.metaData.data);
					const ReflectedType& underLineType = Reflector::GetType(arr.type);
					try
					{
						GetLastJson()[JsonSerializer::CONTAINER_SIZE] = arr.size;

						if (Reflector::isTrivialType(underLineType.typeId))
						{
							std::vector<uint8_t> data;
							data.resize(arr.size * underLineType.size);
							std::memcpy(data.data(), objetPtr, data.size());
							GetLastJson()[JsonSerializer::DATA] = json::binary(data);
						}
						else
						{
							for (size_t i = 0; i < arr.size; i++)
							{
								const size_t offSet = i * underLineType.size;
								m_JsonStack.push_back(&GetLastJson()[std::to_string(i)]);
								SerializeType(objetPtr + offSet, underLineType.typeId);
								m_JsonStack.pop_back();
							}
						}


					}
					catch (...)
					{
						return;
					}
				}
				if constexpr (std::is_same_v<T, ReflectedVector>)
				{
					const ReflectedVector& rVector = std::get<ReflectedVector>(type.metaData.data);
					const ReflectedType& underLineType = Reflector::GetType(rVector.type);
					const std::vector<uint8_t>* ver = reinterpret_cast<const std::vector<uint8_t>*>(objetPtr);
					const size_t typeCount = ver->size() / underLineType.size;

					try
					{
						GetLastJson()[JsonSerializer::CONTAINER_SIZE] = typeCount;
					}
					catch (...)
					{
						return;
					}

					if (Reflector::isTrivialType(underLineType.typeId))
					{

						GetLastJson()[JsonSerializer::DATA] = json::binary(*ver);

					}
					else
					{
						for (size_t i = 0; i < typeCount; i++)
						{
							const size_t offSet = i * underLineType.size;
							m_JsonStack.push_back(&GetLastJson()[std::to_string(i)]);
							SerializeType(ver->data() + offSet, underLineType.typeId);
							m_JsonStack.pop_back();
						}
					}
				}
				if constexpr (std::is_same_v<T, ReflectedString>)
				{
					const ReflectedString& rs = std::get<ReflectedString>(type.metaData.data);
					try
					{
						if (rs.subType == Reflector::GetTypeKey<char>())
						{
							const std::string* s = reinterpret_cast<const std::string*>(objetPtr);
							GetLastJson()[JsonSerializer::CONTAINER_SIZE] = s->size();
							GetLastJson()["string"] = s->c_str();
						}
						else if (rs.subType == Reflector::GetTypeKey<wchar_t>())
						{
							const std::wstring* s = reinterpret_cast<const std::wstring*>(objetPtr);
							GetLastJson()[JsonSerializer::CONTAINER_SIZE] = s->size();
							GetLastJson()["string"] = s->c_str();
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
				if constexpr (std::is_same_v<T, ReflectedMap>)
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
						GetLastJson()[JsonSerializer::CONTAINER_SIZE] = mapSize;
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

							m_JsonStack.push_back(&GetLastJson()[indexs][JsonSerializer::KEY]);
							SerializeType(keyPtr, keyType.typeId);
							m_JsonStack.pop_back();

							m_JsonStack.push_back(&GetLastJson()[indexs][JsonSerializer::VALUE]);
							SerializeType(valuePtr, valueType.typeId);
							m_JsonStack.pop_back();
						}
						catch (...)
						{

						}

						mapBegin++;
					}

				}
				if constexpr (std::is_same_v<T, ReflectedBitSet>)
				{
					GetLastJson()[JsonSerializer::CONTAINER_SIZE] = type.size;
					std::vector<uint8_t> data(type.size);
					std::memcpy(data.data(), objetPtr, data.size());
					GetLastJson()[JsonSerializer::DATA] = json::binary(data);
				}
				if constexpr (std::is_same_v<T, ReflectedSparseSet>)
				{
					const ReflectedSparseSet& reflectedSparSet = std::get<ReflectedSparseSet>(type.metaData.data);

					// dense vector
					m_JsonStack.push_back(&GetLastJson()[JsonSerializer::SPARSE_SET_DENSE]);
					SerializeType(objetPtr + reflectedSparSet.denseVectorOffSet, reflectedSparSet.denseVector);
					m_JsonStack.pop_back();

					// spares vector
					m_JsonStack.push_back(&GetLastJson()[JsonSerializer::SPARSE_SET_SPARSE]);
					SerializeType(objetPtr + reflectedSparSet.spareVectorOffset, reflectedSparSet.spareVector);
					m_JsonStack.pop_back();

				}
				if constexpr (std::is_same_v<T, ReflectedFileSystemPath>)
				{
					const std::filesystem::path& reflectedSparSet = *reinterpret_cast<const std::filesystem::path*>(objetPtr);
					const auto s = reflectedSparSet.generic_string();

					GetLastJson()[JsonSerializer::CONTAINER_SIZE] = s.size();
					GetLastJson()["string"] = s.c_str();
				}
				if constexpr (std::is_same_v<T, ReflectedEnum>)
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

					GetLastJson() = name->c_str();
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

			SerializeMember(member, ptr);
		}
	}
	else
	{
		SerializeTrivial(_typeKey, objetPtr);
	}
}

void JsonSerializer::DeSerializeMember(const PC_CORE::Members& member, uint8_t* objetPtr)
{
	auto& type = PC_CORE::Reflector::GetType(member.typeKey);
	m_JsonStack.push_back(&GetLastJson()[type.name][member.membersName]);
	DeserializeType(objetPtr, member.typeKey);
	m_JsonStack.pop_back();
}

void JsonSerializer::DeserializeType(uint8_t* objetPtr, PC_CORE::TypeId _typeKey)
{
	const ReflectedType& type = Reflector::GetType(_typeKey);
	PERF_REGION_SCOPED_NAME_DYNAMIC(("DeserializeType : " + type.name).c_str());

	if (!std::holds_alternative<std::monostate>(type.metaData.data))
	{
		std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				
				if constexpr (std::is_same_v<T, ReflectedArray>)
				{
					const ReflectedArray& arr = std::get<ReflectedArray>(type.metaData.data);
					const ReflectedType& underLineType = Reflector::GetType(arr.type);

					try
					{
						size_t s = GetLastJson()[JsonSerializer::CONTAINER_SIZE];
						if (GetLastJson()[JsonSerializer::CONTAINER_SIZE] != arr.size)
						{
							PC_LOGERROR("array size missmacht")
								return;
						}
						if (Reflector::isTrivialType(underLineType.typeId))
						{
							std::vector<uint8_t> bytes = GetLastJson()["data"]["bytes"].get<std::vector<uint8_t>>();
							std::memcpy(objetPtr, bytes.data(), arr.size * underLineType.size);
						}
						else
						{
							for (size_t i = 0; i < arr.size; i++)
							{
								const size_t offSet = i * underLineType.size;
								m_JsonStack.push_back(&GetLastJson()[std::to_string(i)]);
								DeserializeType(objetPtr + offSet, underLineType.typeId);
								m_JsonStack.pop_back();
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
							*ver = GetLastJson()[JsonSerializer::DATA]["bytes"].get<std::vector<uint8_t>>();
						}
						else
						{
							const size_t size = GetLastJson()[JsonSerializer::CONTAINER_SIZE];
							ver->resize(size * underLineType.size);
							for (size_t i = 0; i < size; i++)
							{
								const size_t offSet = i * underLineType.size;
								m_JsonStack.push_back(&GetLastJson()[std::to_string(i)]);
								DeserializeType(ver->data() + offSet, underLineType.typeId);
								m_JsonStack.pop_back();
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
							s->resize(GetLastJson()[JsonSerializer::CONTAINER_SIZE]);
							std::string_view v = GetLastJson()["string"];

							memcpy(s->data(), v.data(), s->size());
						}
						else if (rs.subType == Reflector::GetTypeKey<wchar_t>())
						{
							std::wstring* s = reinterpret_cast<std::wstring*>(objetPtr);
							s->resize(GetLastJson()[JsonSerializer::CONTAINER_SIZE]);
							*s = GetLastJson()["string"].template get<typename std::wstring>();
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
						const size_t size = GetLastJson()[JsonSerializer::CONTAINER_SIZE];

						std::string indexs;
						for (size_t i = 0; i < size; i++)
						{
							indexs = std::to_string(i);

							try
							{
								m_JsonStack.push_back(&GetLastJson()[indexs][JsonSerializer::KEY]);
								DeserializeType(keyBuffer.get(), keyType.typeId);
								auto* ref = &(map.*inserFunc)(*keyBuffer.get());
								m_JsonStack.pop_back();

								m_JsonStack.push_back(&GetLastJson()[indexs][JsonSerializer::VALUE]);
								DeserializeType(ref, valueType.typeId);
								m_JsonStack.pop_back();
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
					const size_t s = GetLastJson()[JsonSerializer::CONTAINER_SIZE];
					if (GetLastJson()[JsonSerializer::CONTAINER_SIZE] != type.size)
					{
						PC_LOGERROR("bitset size missmacht")
					}

					std::vector<uint8_t> bytes = GetLastJson()["data"]["bytes"].get<std::vector<uint8_t>>();
					std::memcpy(objetPtr, bytes.data(), type.size);
					return;
				}
				else if constexpr (std::is_same_v<T, ReflectedSparseSet>)
				{
					const ReflectedSparseSet& reflectedSparSet = std::get<ReflectedSparseSet>(type.metaData.data);

					// dense vector
					m_JsonStack.push_back(&GetLastJson()[JsonSerializer::SPARSE_SET_DENSE]);
					DeserializeType(objetPtr + reflectedSparSet.denseVectorOffSet, reflectedSparSet.denseVector);
					m_JsonStack.pop_back();
					// spares vector
					m_JsonStack.push_back(&GetLastJson()[JsonSerializer::SPARSE_SET_SPARSE]);
					DeserializeType(objetPtr + reflectedSparSet.spareVectorOffset, reflectedSparSet.spareVector);
					m_JsonStack.pop_back();
				}
				else if constexpr (std::is_same_v<T, ReflectedFileSystemPath>)
				{
					const std::filesystem::path& reflectedSparSet = *reinterpret_cast<const std::filesystem::path*>(objetPtr);
					std::string s;
					s.resize(GetLastJson()[JsonSerializer::CONTAINER_SIZE]);
					std::string_view v = GetLastJson()["string"];

					memcpy(s.data(), v.data(), s.size());

					std::filesystem::path* p = reinterpret_cast<std::filesystem::path*>(objetPtr);
					*p = std::filesystem::path(std::move(s));
				}
				else if constexpr (std::is_same_v<T, ReflectedEnum>)
				{
					//  TODO
					const ReflectedEnum& renum = std::get<ReflectedEnum>(type.metaData.data);
					std::string_view v = GetLastJson(); // get the enum as string
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
			DeSerializeMember(member, ptr);
		}
	}
	else
	{
		DeSerializeTrivial(_typeKey, objetPtr);
	}
}

