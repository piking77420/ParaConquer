#include "Serialize/JsonSerializer.hpp"

#include <fstream>
#include <any>
#include <unordered_map>
#include <variant>

#include "Reflection/Reflector.hpp"

#include "Resources/Resource.hpp"
#include "ObjectPtr.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/StaticMesh.hpp"

using namespace PC_CORE;

void JsonSerializer::SerializeTrivial(TypeId id, const uint8_t* objetPtr)
{
    if (Reflector::IsTypeIdIs<bool>(id))
    {
        auto b = reinterpret_cast<const bool*>(objetPtr);
        WriteJson(*b) ? boolAlpha1s : boolAlpha0s;
    }
    else if (Reflector::IsTypeIdIs<char>(id))
    {
        auto b = reinterpret_cast<const char*>(objetPtr);
        WriteJson(*b);
    }
    else if (Reflector::IsTypeIdIs<int8_t>(id))
    {
        auto b = reinterpret_cast<const int8_t*>(objetPtr);
        WriteJson(*b);
    }
    else if (Reflector::IsTypeIdIs<int16_t>(id))
    {
        auto b = reinterpret_cast<const int16_t*>(objetPtr);
        WriteJson(*b);
    }
    else if (Reflector::IsTypeIdIs<int32_t>(id))
    {
        auto b = reinterpret_cast<const int32_t*>(objetPtr);
        WriteJson(*b);
    }
    else if (Reflector::IsTypeIdIs<int64_t>(id))
    {
        auto b = reinterpret_cast<const int64_t*>(objetPtr);
        WriteJson(*b);
    }
    else if (Reflector::IsTypeIdIs<int>(id))
    {
        auto b = reinterpret_cast<const int*>(objetPtr);
        WriteJson(*b);
    }
    else if (Reflector::IsTypeIdIs<uint8_t>(id))
    {
        auto b = objetPtr;
        WriteJson(*b);
    }
    else if (Reflector::IsTypeIdIs<uint16_t>(id))
    {
        auto b = reinterpret_cast<const uint16_t*>(objetPtr);
        WriteJson(*b);
    }
    else if (Reflector::IsTypeIdIs<uint32_t>(id))
    {
        auto b = reinterpret_cast<const uint32_t*>(objetPtr);
        WriteJson(*b);
    }
    else if (Reflector::IsTypeIdIs<uint64_t>(id))
    {
        auto b = reinterpret_cast<const uint64_t*>(objetPtr);
        WriteJson(*b);
    }
    else if (Reflector::IsTypeIdIs<float>(id))
    {
        auto f = reinterpret_cast<const float*>(objetPtr);
        WriteJson(*f);
    }
    else if (Reflector::IsTypeIdIs<double>(id))
    {
        auto f = reinterpret_cast<const double*>(objetPtr);
        WriteJson(*f) ;
    }
    else
    {
        assert(false);
    }
}

void JsonSerializer::DeSerializeTrivial(TypeId id, uint8_t* objetPtr)
{
    PERF_REGION_SCOPED

    if (Reflector::IsTypeIdIs<bool>(id))
    {
        auto b = reinterpret_cast<bool*>(objetPtr);
        std::string get;
        if (ReadJson(get))
        {
            if (strcmp(boolAlpha1s, get.c_str()) == 0) *b = true;
            else if (strcmp(boolAlpha0s, get.c_str()) == 0) *b = false;
            else
                assert(false);
        }
        
    }
    else if (Reflector::IsTypeIdIs<char>(id))
    {
        auto b = reinterpret_cast<char*>(objetPtr);
        char get{}; 
        if (ReadJson(get))
        {
            *b = get;
        }

    }
    else if (Reflector::IsTypeIdIs<int8_t>(id))
    {
        auto b = reinterpret_cast<int8_t*>(objetPtr);
        int8_t get{}; 
        if (ReadJson(get))
        {
            *b = get;
        }

    }
    else if (Reflector::IsTypeIdIs<int16_t>(id))
    {
        auto b = reinterpret_cast<int16_t*>(objetPtr);
        int16_t get{}; 
        if (ReadJson(get))
        {
            *b = get;

        }

    }
    else if (Reflector::IsTypeIdIs<int32_t>(id))
    {
        auto b = reinterpret_cast<int32_t*>(objetPtr);
        int32_t get{}; 
        if (ReadJson(get))
        {
            *b = get;
        }

    }
    else if (Reflector::IsTypeIdIs<int64_t>(id))
    {
        auto b = reinterpret_cast<int64_t*>(objetPtr);
        int64_t get{}; 
        if (ReadJson(get))
        {
            *b = get;

        }

    }
    else if (Reflector::IsTypeIdIs<uint8_t>(id))
    {
        auto b = objetPtr;
        uint8_t get{}; 
        if (ReadJson(get))
        {
            *b = get;
        }

    }
    else if (Reflector::IsTypeIdIs<uint16_t>(id))
    {
        auto b = reinterpret_cast<uint16_t*>(objetPtr);
        uint16_t get{}; 
        if (ReadJson(get))
        {
            *b = get;
        }

    }
    else if (Reflector::IsTypeIdIs<uint32_t>(id))
    {
        auto b = reinterpret_cast<uint32_t*>(objetPtr);
        uint32_t get{}; 
        if (ReadJson(get))
        {
            *b = get;
        }

    }
    else if (Reflector::IsTypeIdIs<uint64_t>(id))
    {
        auto b = reinterpret_cast<uint64_t*>(objetPtr);
        uint64_t get{}; 
        if (ReadJson(get))
        {
            *b = get;
        }

    }
    else if (Reflector::IsTypeIdIs<float>(id))
    {
        auto f = reinterpret_cast<float*>(objetPtr);
        float get{}; 
        if (ReadJson(get))
        {
            *f = get;
        }

    }
    else if (Reflector::IsTypeIdIs<double>(id))
    {
        auto f = reinterpret_cast<double*>(objetPtr);
        double get{}; 
        if (ReadJson(get))
        {
            *f = get;
        }

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
}

void JsonSerializer::DeSerializing(uint8_t* _objetPtr, TypeId _typeKey)
{
    PERF_REGION_SCOPED

    const ReflectedType& type = Reflector::GetType(_typeKey);
    //Entry
    m_JsonStack.push_back(&m_MainJson[type.name]);
    DeserializeType(_objetPtr, _typeKey);
    m_JsonStack.pop_back();
}


bool JsonSerializer::IsOpen() const
{
    switch (m_SerializeOperation)
    {
    case SerializeOperation::Serialize:
        return m_OutStream.is_open();
    case SerializeOperation::DeSerialize:
        return m_Instream.is_open();
    default: ;
    }

    return false;
}


void JsonSerializer::SerializeMember(const Members& member, const uint8_t* objetPtr)
{
    PERF_REGION_SCOPED

    if (member.memberFlag & Members::EnumFlag::Serialize)
        return;

    if (auto* json = GetLastJson())
    {
        auto& type = Reflector::GetType(member.typeKey);
        m_JsonStack.push_back(&(*json)[type.name][member.membersName]);
        SerializeType(objetPtr, member.typeKey);
        m_JsonStack.pop_back();
    }
    
}

void JsonSerializer::SerializeType(const uint8_t* objetPtr, TypeId _typeKey)
{
    const ReflectedType& type = Reflector::GetType(_typeKey);
    PERF_REGION_SCOPED_NAME_DYNAMIC(("SerializeType : " + type.name).c_str());

    if (!std::holds_alternative<std::monostate>(type.metaData.data))
    {
        std::visit([&](auto&& arg)
        {
            using T = std::decay_t<decltype(arg)>;


            if constexpr (std::is_same_v<T, ReflectedObjPtr>)
            {
                const ReflectedObjPtr& reflectedObjPtr = std::get<ReflectedObjPtr>(type.metaData.data);
                const auto& objectPtrR = reinterpret_cast<const ObjectPtr<Resource>&>(*objetPtr);

                if (auto* json = GetLastJson(); objectPtrR)
                {
                    m_JsonStack.push_back(&(*json)[RESOURCE_OBJECT]);

                    (*json)[OBJECT_TYPE] = objectPtrR->GetTypeKey();
                    SerializeType(reinterpret_cast<const uint8_t*>(objectPtrR.get()), objectPtrR->GetTypeKey());
                    objectPtrR->AfterSerialize(this);

                    m_JsonStack.pop_back();
                }
            }
            else if constexpr (std::is_same_v<T, ReflectedWeakObjPtr>)
            {
                const auto& wobjectPtrR = reinterpret_cast<const WeakObjectPtr<Resource>&>(*objetPtr);

                if (auto* json = GetLastJson(); wobjectPtrR.IsValid())
                {
                    const auto& guid = reinterpret_cast<const Guid&>(wobjectPtrR.Lock().get()->GetGuid());
                    m_JsonStack.push_back(&(*json)[GUID_KEY]);
                    SerializeType(reinterpret_cast<const uint8_t*>(&guid), Reflector::GetTypeKey<Guid>());
                    m_JsonStack.pop_back();
                }
            }
            else if constexpr (std::is_same_v<T, ReflectedArray>)
            {
                const ReflectedArray& arr = std::get<ReflectedArray>(type.metaData.data);
                const ReflectedType& underLineType = Reflector::GetType(arr.type);
                if (auto* json = GetLastJson())
                {
                    (*json)[CONTAINER_SIZE] = arr.size;

                    if (Reflector::isTrivialType(underLineType.typeId))
                    {
                        std::vector<uint8_t> data;
                        data.resize(arr.size * underLineType.size);
                        std::memcpy(data.data(), objetPtr, data.size());
                        (*json)[DATA] = json::binary(data);
                    }
                    else
                    {
                        for (size_t i = 0; i < arr.size; i++)
                        {
                            const size_t offSet = i * underLineType.size;
                            m_JsonStack.push_back(&(*json)[std::to_string(i)]);
                            SerializeType(objetPtr + offSet, underLineType.typeId);
                            m_JsonStack.pop_back();
                        }
                    }
                }
            }
            if constexpr (std::is_same_v<T, ReflectedVector>)
            {
                const ReflectedVector& rVector = std::get<ReflectedVector>(type.metaData.data);
                const ReflectedType& underLineType = Reflector::GetType(rVector.type);
                auto ver = reinterpret_cast<const std::vector<uint8_t>*>(objetPtr);
                const size_t typeCount = ver->size() / underLineType.size;

                if (auto* json = GetLastJson())
                {
                    (*json)[CONTAINER_SIZE] = typeCount;

                    if (Reflector::isTrivialType(underLineType.typeId))
                    {
                        (*json)[DATA] = json::binary(*ver);
                    }
                    else
                    {
                        for (size_t i = 0; i < typeCount; i++)
                        {
                            const size_t offSet = i * underLineType.size;
                            m_JsonStack.push_back(&(*json)[std::to_string(i)]);
                            SerializeType(ver->data() + offSet, underLineType.typeId);
                            m_JsonStack.pop_back();
                        }
                    }
                }
      

                
            }
            if constexpr (std::is_same_v<T, ReflectedString>)
            {
                const ReflectedString& rs = std::get<ReflectedString>(type.metaData.data);
                if (auto* json = GetLastJson())
                {
                    if (rs.subType == Reflector::GetTypeKey<char>())
                    {
                        auto s = reinterpret_cast<const std::string*>(objetPtr);
                        (*json)[CONTAINER_SIZE] = s->size();
                        (*json)["string"] = s->c_str();
                    }
                    else if (rs.subType == Reflector::GetTypeKey<wchar_t>())
                    {
                        auto s = reinterpret_cast<const std::wstring*>(objetPtr);
                        (*json)[CONTAINER_SIZE] = s->size();
                        (*json)["string"] = s->c_str();
                    }
                    else
                    {
                        assert(false);
                    }
                }

            }
            if constexpr (std::is_same_v<T, ReflectedMap>)
            {
                const ReflectedMap& reflectedMap = std::get<ReflectedMap>(type.metaData.data);
                auto dirtyPtr = const_cast<uint8_t*>(objetPtr);
                auto map = reinterpret_cast<UnordoredByteMap*>(dirtyPtr);

                const ReflectedType& keyType = Reflector::GetType(reflectedMap.key);
                const ReflectedType& valueType = Reflector::GetType(reflectedMap.value);
                const ReflectMapFunction& reflectMapFunction = Reflector::m_UnordoredMapReflectFunction.at(type.typeId);


                const size_t mapSize = map->size();
                if (auto* json = GetLastJson())
                {
                    (*json)[CONTAINER_SIZE] = mapSize;
                }
                
                std::string indexs;

                UnorderedMapUnrefConstIteratorFunc unrefFunf = nullptr;
                std::memcpy(&unrefFunf, &reflectMapFunction.unrefFunc, sizeof(UnorderedMapUnrefConstIteratorFunc));

                auto mapBegin = map->begin();
                for (size_t i = 0; i < mapSize; i++)
                {
                    indexs = std::to_string(i);

                    auto* pair = (mapBegin.*unrefFunf)();
                    if (auto* json = GetLastJson())
                    {
                        auto keyPtr = reinterpret_cast<const uint8_t*>(pair);
                        const uint8_t* valuePtr = keyPtr + reflectedMap.offsetBetweenKeyAndValueInPair;

                        m_JsonStack.push_back(&(*json)[indexs][KEY]);
                        SerializeType(keyPtr, keyType.typeId);
                        m_JsonStack.pop_back();

                        m_JsonStack.push_back(&(*json)[indexs][VALUE]);
                        SerializeType(valuePtr, valueType.typeId);
                        m_JsonStack.pop_back();
                    }
              

                    ++mapBegin;
                }
            }
            if constexpr (std::is_same_v<T, ReflectedBitSet>)
            {
                if (auto* json = GetLastJson())
                {
                    (*json)[CONTAINER_SIZE] = type.size;
                    std::vector<uint8_t> data(type.size);
                    std::memcpy(data.data(), objetPtr, data.size());
                    (*json)[DATA] = json::binary(data);
                }
                
            }
            if constexpr (std::is_same_v<T, ReflectedSparseSet>)
            {
                if (auto* json = GetLastJson())
                {
                    const ReflectedSparseSet& reflectedSparSet = std::get<ReflectedSparseSet>(type.metaData.data);

                    // dense vector
                    m_JsonStack.push_back(&(*json)[SPARSE_SET_DENSE]);
                    SerializeType(objetPtr + reflectedSparSet.denseVectorOffSet, reflectedSparSet.denseVector);
                    m_JsonStack.pop_back();

                    // spares vector
                    m_JsonStack.push_back(&(*json)[SPARSE_SET_SPARSE]);
                    SerializeType(objetPtr + reflectedSparSet.spareVectorOffset, reflectedSparSet.spareVector);
                    m_JsonStack.pop_back();
                }
                
            }
            if constexpr (std::is_same_v<T, ReflectedFileSystemPath>)
            {
                if (auto* json = GetLastJson())
                {
                    const std::filesystem::path& reflectedSparSet = *reinterpret_cast<const std::filesystem::path*>(
                        objetPtr);
                    const auto s = reflectedSparSet.generic_string();

                    (*json)[CONTAINER_SIZE] = s.size();
                    (*json)["string"] = s.c_str();
                }
                
            }
            if constexpr (std::is_same_v<T, ReflectedEnum>)
            {
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
                if (auto* json = GetLastJson())
                {
                    *json = name->c_str();
                }
            }
        }, type.metaData.data);
        return;
    }


    if (type.typeFlags & ReflectedType::TypeFlagBits::Composite)
    {
        for (auto& member : type.metaData.members)
        {
            if (member.memberFlag & Members::EnumFlag::Serialize)
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

void JsonSerializer::DeSerializeMember(const Members& member, uint8_t* objetPtr)
{
    auto& type = Reflector::GetType(member.typeKey);
    if (auto* json = GetLastJson())
    {
        m_JsonStack.push_back(&(*json)[type.name][member.membersName]);
        DeserializeType(objetPtr, member.typeKey);
        m_JsonStack.pop_back();
    }
}

void JsonSerializer::DeserializeType(uint8_t* objetPtr, TypeId _typeKey)
{
    const ReflectedType& type = Reflector::GetType(_typeKey);
    PERF_REGION_SCOPED_NAME_DYNAMIC(("DeserializeType : " + type.name).c_str());

    if (!std::holds_alternative<std::monostate>(type.metaData.data))
    {
        std::visit([&](auto&& arg)
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, ReflectedObjPtr>)
            {
                const ReflectedObjPtr& reflectedObjPtr = std::get<ReflectedObjPtr>(type.metaData.data);
                auto& objectPtrR = reinterpret_cast<ObjectPtr<Resource>&>(*objetPtr);
                if (auto* json = GetLastJson())
                {
                    if (json->contains(GUID_KEY))
                    {
                        m_JsonStack.push_back(&(*json)[RESOURCE_OBJECT]);

                        const TypeId id = (*json)[OBJECT_TYPE].get<TypeId>();

                        if (Reflector::Exist(id))
                        {
                            const ReflectedType& t = Reflector::GetType(id);
                            const DeleteFunc deleteFunc = t.metaData.deleteFunc;

                            // Scary Ptr and Vtable manipaluation
                            auto reconstructObject = new uint8_t[t.size];
                            objectPtrR = std::reinterpret_pointer_cast<Resource>(std::shared_ptr<uint8_t[]>(
                                reconstructObject, [deleteFunc](uint8_t* rDelted)
                                {
                                    deleteFunc(rDelted);
                                }));


                            DeserializeType(reconstructObject, id);
                            objectPtrR->AfterDeSerialize(this);
                        }
                        else
                        {
                            PC_LOGERROR("Faile to Serialize ReflectedObjPtr")
                        }

                        m_JsonStack.pop_back();
                    }
                }
            }
            else if constexpr (std::is_same_v<T, ReflectedWeakObjPtr>)
            {
                auto& wobjectPtrR = reinterpret_cast<WeakObjectPtr<Resource>&>(*objetPtr);
                if (auto* json = GetLastJson())
                {
                    if (json->contains(GUID_KEY))
                    {
                        Guid guid;
                        m_JsonStack.push_back(&(*json)[GUID_KEY]);
                        DeserializeType(reinterpret_cast<uint8_t*>(&guid), Reflector::GetTypeKey<Guid>());
                        m_JsonStack.pop_back();
                        if (ResourceManager::Exist(guid))
                        {
                            wobjectPtrR = WeakObjectPtr<Resource>(ResourceManager::Get<Resource>(guid));
                        }
                    }
                }
            }
            else if constexpr (std::is_same_v<T, ReflectedArray>)
            {
                const ReflectedArray& arr = std::get<ReflectedArray>(type.metaData.data);
                const ReflectedType& underLineType = Reflector::GetType(arr.type);

                if (auto* json = GetLastJson())
                {
                    size_t s = (*json)[CONTAINER_SIZE];
                    if ((*json)[CONTAINER_SIZE] != arr.size)
                    {
                        PC_LOGERROR("array size missmacht")
                            return;
                    }
                    if (Reflector::isTrivialType(underLineType.typeId))
                    {
                        auto bytes = (*json)["data"]["bytes"].get<std::vector<uint8_t>>();
                        std::memcpy(objetPtr, bytes.data(), arr.size * underLineType.size);
                    }
                    else
                    {
                        for (size_t i = 0; i < arr.size; i++)
                        {
                            const size_t offSet = i * underLineType.size;
                            m_JsonStack.push_back(&(*json)[std::to_string(i)]);
                            DeserializeType(objetPtr + offSet, underLineType.typeId);
                            m_JsonStack.pop_back();
                        }
                    }
                }
                
            }
            else if constexpr (std::is_same_v<T, ReflectedVector>)
            {
                auto ver = reinterpret_cast<std::vector<uint8_t>*>(objetPtr);
                const ReflectedVector& vector = std::get<ReflectedVector>(type.metaData.data);
                const ReflectedType& underLineType = Reflector::GetType(vector.type);
                if (auto* json = GetLastJson())
                {
                    if (Reflector::isTrivialType(underLineType.typeId))
                    {
                        *ver = (*json)[DATA]["bytes"].get<std::vector<uint8_t>>();
                    }
                    else
                    {
                        const size_t size = (*json)[CONTAINER_SIZE];
                        ver->resize(size * underLineType.size);
                        for (size_t i = 0; i < size; i++)
                        {
                            const size_t offSet = i * underLineType.size;
                            m_JsonStack.push_back(&(*json)[std::to_string(i)]);
                            DeserializeType(ver->data() + offSet, underLineType.typeId);
                            m_JsonStack.pop_back();
                        }
                    }

                }
                    
            }
            else if constexpr (std::is_same_v<T, ReflectedString>)
            {
                const ReflectedString& rs = std::get<ReflectedString>(type.metaData.data);
                if (auto* json = GetLastJson())
                {
                    if (rs.subType == Reflector::GetTypeKey<char>())
                    {
                        auto s = reinterpret_cast<std::string*>(objetPtr);
                        s->resize((*json)[CONTAINER_SIZE]);
                        std::string_view v = (*json)["string"];

                        memcpy(s->data(), v.data(), s->size());
                    }
                    else if (rs.subType == Reflector::GetTypeKey<wchar_t>())
                    {
                        auto s = reinterpret_cast<std::wstring*>(objetPtr);
                        s->resize((*json)[CONTAINER_SIZE]);
                        *s = (*json)["string"].get<std::wstring>();
                    }
                    else
                    {
                        assert(false);
                    }
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
                auto keyBuffer = std::make_unique<uint8_t[]>(keyType.size);

               
                    if (auto* json = GetLastJson())
                    {
                        const size_t size = (*json)[CONTAINER_SIZE];

                        std::string indexs;
                        for (size_t i = 0; i < size; i++)
                        {
                            indexs = std::to_string(i);

                            m_JsonStack.push_back(&(*json)[indexs][KEY]);
                            DeserializeType(keyBuffer.get(), keyType.typeId);
                            auto* ref = &(map.*inserFunc)(*keyBuffer.get());
                            m_JsonStack.pop_back();

                            m_JsonStack.push_back(&(*json)[indexs][VALUE]);
                            DeserializeType(ref, valueType.typeId);
                            m_JsonStack.pop_back();
                        }
                    }
                    
               
            }
            else if constexpr (std::is_same_v<T, ReflectedBitSet>)
            {
                if (auto* json = GetLastJson())
                {
                    const size_t s = (*json)[CONTAINER_SIZE];
                    if ((*json)[CONTAINER_SIZE] != type.size)
                    {
                        PC_LOGERROR("bitset size missmacht")
                    }

                    auto bytes = (*json)["data"]["bytes"].get<std::vector<uint8_t>>();
                    std::memcpy(objetPtr, bytes.data(), type.size);
                }
                
            }
            else if constexpr (std::is_same_v<T, ReflectedSparseSet>)
            {
                const ReflectedSparseSet& reflectedSparSet = std::get<ReflectedSparseSet>(type.metaData.data);

                if (auto* json = GetLastJson())
                {
                    // dense vector
                    m_JsonStack.push_back(&(*json)[SPARSE_SET_DENSE]);
                    DeserializeType(objetPtr + reflectedSparSet.denseVectorOffSet, reflectedSparSet.denseVector);
                    m_JsonStack.pop_back();
                    // spares vector
                    m_JsonStack.push_back(&(*json)[SPARSE_SET_SPARSE]);
                    DeserializeType(objetPtr + reflectedSparSet.spareVectorOffset, reflectedSparSet.spareVector);
                    m_JsonStack.pop_back();
                }
            }
            else if constexpr (std::is_same_v<T, ReflectedFileSystemPath>)
            {
                const std::filesystem::path& reflectedSparSet = *reinterpret_cast<const std::filesystem::path*>(
                    objetPtr);
                std::string s;
                if (auto* json = GetLastJson())
                {
                    s.resize((*json)[CONTAINER_SIZE]);
                    std::string_view v = (*json)["string"];

                    memcpy(s.data(), v.data(), s.size());

                    auto p = reinterpret_cast<std::filesystem::path*>(objetPtr);
                    *p = std::filesystem::path(std::move(s));
                }
            }
            else if constexpr (std::is_same_v<T, ReflectedEnum>)
            {
                //  TODO
                const ReflectedEnum& renum = std::get<ReflectedEnum>(type.metaData.data);
                std::string_view v;
                if (ReadJson(v))
                {
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

               
            }
            else if constexpr (std::is_same_v<T, std::monostate>)
            {
            }
        }, type.metaData.data);
        return;
    }


    if (type.typeFlags & ReflectedType::TypeFlagBits::Composite)
    {
        if (type.metaData.createFunc != nullptr)
            type.metaData.createFunc(objetPtr);

        for (auto& member : type.metaData.members)
        {
            if (member.memberFlag & Members::EnumFlag::Serialize)
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

bool JsonSerializer::OpenFile(const std::string& _path, SerializeOperation _operation)
{
    Serializer::OpenFile(_path, _operation);

    switch (m_SerializeOperation)
    {
    case SerializeOperation::Serialize:
        {
            PERF_REGION_SCOPED;
            m_MainJson = {};

            m_OutStream = std::ofstream(m_CurrentFilePath);

            if (!m_OutStream.is_open())
            {
                PC_LOG("Failed to open file {}", m_CurrentFilePath);
                return false;
            }
        }
        break;
    case SerializeOperation::DeSerialize:
        {
            m_MainJson = {};

            m_Instream = std::ifstream(m_CurrentFilePath);

            if (!m_Instream.is_open())
            {
                m_Instream.close();
                PC_LOGERROR("Failed to open file {}", m_CurrentFilePath);
                return false;
            }

            {
                PERF_REGION_SCOPED_NAMED("Parse JSON");
                try
                {
                    m_MainJson = json::parse(m_Instream);
                }
                catch (const std::exception&)
                {
                }
            }
        }
        break;
    default: ;
    }

    return true;
}

bool JsonSerializer::CloseFile()
{
    PERF_REGION_SCOPED;

    assert(m_JsonStack.empty());

    switch (m_SerializeOperation)
    {
    case SerializeOperation::Serialize:
        {
            {
                PERF_REGION_SCOPED_NAMED("Dump JSON");
                m_OutStream << std::setw(4) << m_MainJson.dump(4);
            }

            m_OutStream.close();
            m_OutStream = {};
        }
        break;
    case SerializeOperation::DeSerialize:
        {
            PERF_REGION_SCOPED
            m_MainJson = {};
            m_Instream.close();
            m_Instream = {};
        }
        break;
    default: ;
    }

    Serializer::CloseFile();
    return true;
}

void JsonSerializer::SerializeCompactBuffer(const char* _key, const CompactBuffer& _compactBuffer)
{
    m_JsonStack.push_back(&m_MainJson[_key]);
    SerializeType(reinterpret_cast<const uint8_t*>(&_compactBuffer), Reflector::GetTypeKey<CompactBuffer>());
    m_JsonStack.pop_back();
}

void JsonSerializer::DeSerializeCompactBuffer(const char* _key, CompactBuffer* _compactBuffer)
{
    m_JsonStack.push_back(&m_MainJson[_key]);
    DeserializeType(reinterpret_cast<uint8_t*>(_compactBuffer), Reflector::GetTypeKey<CompactBuffer>());
    m_JsonStack.pop_back();
}
