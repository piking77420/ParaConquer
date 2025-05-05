#include "serialize/serializer.h"


#include <any>

#include "GLFW/glfw3native.h"
#include "resources/resource.hpp"
#include "resources/resource_manager.hpp"
#include <unordered_map>
#include <ecs/component_array.hpp>

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


#define CONTAINER_SIZE "size"
#define RESOURCE_PTR_TYPE "resourceType"
#define KEY "key"
#define VALUE "value"

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
    if (member.memberFlag & MemberEnumFlag::NOTSERIALIZE)
        return;

    auto& type = PC_CORE::Reflector::GetType(member.typeKey);
    json& memberJson = _jsonFile[type.name][member.membersName];

    SerializeType(memberJson, objetPtr, member.typeKey);
  
}



void SerializeType(json& _jsonFile ,const uint8_t* objetPtr, TypeId _typeKey)
{
    const ReflectedType& type = Reflector::GetType(_typeKey);

    switch (type.metaData.typeNatureMetaData.metaDataTypeEnum)
    {
    case TypeNatureMetaDataEnum::ResourceRefType:
    {
        uint64_t ptr = (uint64_t)objetPtr;
        auto& typeRef = Reflector::GetType(type.metaData.typeNatureMetaData.metaDataType.resourceRef.type);

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
                    _jsonFile["Guid"] = std::string(resourceSPtr->guid);
                }
                else
                {
                    _jsonFile["Guid"] = resourceSPtr->guid.Empty();
                }
            }
            catch (...)
            {
                return;
            }

        }
        return;
    }
    case TypeNatureMetaDataEnum::ResourceHandle : 
    {
        auto& pointedType = Reflector::GetType(type.metaData.typeNatureMetaData.metaDataType.resourceHandleType.type);
        
            const std::shared_ptr<Resource>* rsPtr = reinterpret_cast<const std::shared_ptr<Resource>*>(objetPtr);
            const ResourceInterface<Resource>* rsInterfaceDummie = reinterpret_cast<const ResourceInterface<Resource>*>(rsPtr->get());

            assert(rsInterfaceDummie->GetType().typeId != Reflector::GetTypeKey<Resource>());
            try
            {
                _jsonFile[RESOURCE_PTR_TYPE] = rsInterfaceDummie->GetType().typeId;
            }
            catch (...)
            {
                return;
            }

            SerializeType(_jsonFile, reinterpret_cast<const uint8_t*>(rsPtr->get()), pointedType.typeId);
        return;
    }
    case TypeNatureMetaDataEnum::String:
    {
        const RelfectedString& rs = type.metaData.typeNatureMetaData.metaDataType.relfectedString;
        const TypeId& underLineType = rs.type;

        try
        {
            if (rs.type == Reflector::GetTypeKey<char>())
            {
                const std::string* s = reinterpret_cast<const std::string*>(objetPtr);
                _jsonFile[CONTAINER_SIZE] = s->size();
                _jsonFile["string"] = s->c_str();
            }
            else if (rs.type == Reflector::GetTypeKey<wchar_t>())
            {
                const std::wstring* s = reinterpret_cast<const std::wstring*>(objetPtr);
                _jsonFile[CONTAINER_SIZE] = s->size();
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

      
        return;
    }
    case TypeNatureMetaDataEnum::Array:
    {

        const Array& arr = type.metaData.typeNatureMetaData.metaDataType.array;
        const ReflectedType& underLineType = Reflector::GetType(arr.type);
        const size_t typeCount = arr.size / underLineType.size;
        try
        {
            _jsonFile[CONTAINER_SIZE] = typeCount;

            for (size_t i = 0; i < typeCount; i++)
            {
                const size_t offSet = i * underLineType.size;
                SerializeType(_jsonFile[std::to_string(i)], objetPtr + offSet, underLineType.typeId);
            }
        }
        catch (...)
        {
            return;
        }

        return;
    }
    case TypeNatureMetaDataEnum::Vector:
    {
       
        const std::vector<uint8_t>* ver = reinterpret_cast<const std::vector<uint8_t>*>(objetPtr);
        const ReflectedType& underLineType = Reflector::GetType(type.metaData.typeNatureMetaData.metaDataType.vector.type);
        const size_t typeCount = ver->size() / underLineType.size;

        try
        {
            _jsonFile[CONTAINER_SIZE] = typeCount;
        }
        catch (...)
        {
            return;
        }

        for (size_t i = 0; i < typeCount; i++)
        {
            const size_t offSet = i * underLineType.size;
            SerializeType(_jsonFile[std::to_string(i)], ver->data() + offSet, underLineType.typeId);
        }

        return;
    }
    return;
    case TypeNatureMetaDataEnum::Map:
    {
       
       
        return;
    }
    case TypeNatureMetaDataEnum::UnordoredMap:
    {
        uint8_t* dirtyPtr = const_cast<uint8_t*>(objetPtr);
        UnordoredByteMap* map = reinterpret_cast<UnordoredByteMap*>(dirtyPtr);
        const ReflectedMap& reflectedMap = type.metaData.typeNatureMetaData.metaDataType.mapReflected;

        const ReflectedType& keyType = Reflector::GetType(reflectedMap.key);
        const ReflectedType& valueType = Reflector::GetType(reflectedMap.value);
        const ReflectMapFunction& reflectMapFunction = Reflector::m_UnordoredMapReflectFunction.at(type.typeId);


        const size_t mapSize = map->size();
        try
        {
            _jsonFile[CONTAINER_SIZE] = mapSize;
        }
        catch (...)
        {
            return;
        }
        std::string indexs;

        const std::unordered_map<TypeId, ComponentArray>* realPtr = reinterpret_cast<const std::unordered_map<TypeId, ComponentArray>*>(objetPtr);


        UnorderedMapUnrefConstIteratorFunc unrefFunf = nullptr;
        std::memcpy(&unrefFunf, &reflectMapFunction.unrefFunc, sizeof(UnorderedMapUnrefConstIteratorFunc));


        auto itTrue = realPtr->begin();
        auto itTrue2 = realPtr->begin();

        auto& itfalse = *reinterpret_cast<decltype(map->begin())*>(&itTrue);
 

        assert((uint64_t)&itTrue->second == (uint64_t)&itTrue2->second);
        assert((uint64_t)&itTrue->second == (uint64_t)&itTrue2->second);


        for (size_t i = 0; i < mapSize; i++)
        {
            
            indexs = std::to_string(i);
           

            const std::shared_ptr<Resource>* rsPtr = reinterpret_cast<const std::shared_ptr<Resource>*>((uint8_t*)(&itfalse->first + keyType.size));
            const ResourceInterface<Resource>* rsInterfaceDummie = reinterpret_cast<const ResourceInterface<Resource>*>(rsPtr->get());
      
           //verificatino // assert((uint8_t*)(&itfalse->first + keyType.size) == (uint8_t*)(bytePair + keyType.size));
            
            //assert((uint64_t)& itTrue->second == (uint64_t)(&((itfalse.*unrefFunf)()->second)));

            static_assert(sizeof(std::pair<std::string, std::shared_ptr<Resource>>) == sizeof(std::string) + sizeof(std::shared_ptr<Resource >));
            try
            {
                SerializeType(_jsonFile[indexs][KEY], (uint8_t*)&itfalse->first , keyType.typeId);

               const ComponentArray* componentArrayFalse = reinterpret_cast<const ComponentArray*>((uint8_t*)(&itfalse->first + keyType.size));
               SerializeType(_jsonFile[indexs][VALUE], (uint8_t*)(&itfalse->first + keyType.size), valueType.typeId);
            }
            catch (...)
            {
               
            }
           
            itfalse++;

            itTrue++;
        }


        return;
    }
    case TypeNatureMetaDataEnum::None:
    default:
        break;
    }


    if (type.typeFlags & TypeFlagBits::COMPOSITE)
    {
        for (auto& member : type.metaData.members)
        {
            if (member.memberFlag & MemberEnumFlag::NOTSERIALIZE)
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


void PC_CORE::Serializer::Serializing(const uint8_t* objetPtr, const fs::path& _fileToSerialize, TypeId _typeKey)
{
    std::ofstream myfile(_fileToSerialize.generic_string());

    if (!myfile.is_open()) 
    {
        return;
    }
    const ReflectedType& type = Reflector::GetType(_typeKey);
    json j;
    SerializeType(j[type.name], objetPtr, _typeKey);

    myfile << std::setw(4) << j.dump(4);
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

    switch (type.metaData.typeNatureMetaData.metaDataTypeEnum)
    {
    case TypeNatureMetaDataEnum::ResourceRefType:
    {
        uint64_t ptr = (uint64_t)objetPtr;

        auto& typeRef = Reflector::GetType(type.metaData.typeNatureMetaData.metaDataType.resourceRef.type);
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
                    std::shared_ptr<Resource> r = PC_CORE::ResourceManager::GetByGuid(g);
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
        return;
    }
    case TypeNatureMetaDataEnum::ResourceHandle:
    {
        auto& pointedType = Reflector::GetType(type.metaData.typeNatureMetaData.metaDataType.resourceHandleType.type);
        try
        {
            TypeId pointedTypeId = _jsonFile[RESOURCE_PTR_TYPE];
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
       
        return;
    }
    case TypeNatureMetaDataEnum::String:
    {
        const RelfectedString& rs = type.metaData.typeNatureMetaData.metaDataType.relfectedString;
        const TypeId& underLineType = rs.type;

        try
        {
            if (rs.type == Reflector::GetTypeKey<char>())
            {
                std::string* s = reinterpret_cast<std::string*>(objetPtr);
                s->resize(_jsonFile[CONTAINER_SIZE]);
                std::string_view v = _jsonFile["string"];

                memcpy(s->data(), v.data(), s->size());
            }
            else if (rs.type == Reflector::GetTypeKey<wchar_t>())
            {
                std::wstring* s = reinterpret_cast<std::wstring*>(objetPtr);
                s->resize(_jsonFile[CONTAINER_SIZE]);
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
       
        return;
    }
    case TypeNatureMetaDataEnum::Array:
        {
            const Array& arr = type.metaData.typeNatureMetaData.metaDataType.array;
            const ReflectedType& underLineType = Reflector::GetType(arr.type);
            const size_t typeCount = arr.size / underLineType.size;

            try
            {
                if (_jsonFile[CONTAINER_SIZE] != typeCount)
                {
                    PC_LOGERROR("array size missmacht")
                        return;
                }

                for (size_t i = 0; i < typeCount; i++)
                {
                    const size_t offSet = i * underLineType.size;
                    DeserializeType(_jsonFile[std::to_string(i)], objetPtr + offSet, underLineType.typeId);
                }
            }
            catch (...)
            {

            }
           

            return;
        }
        break;
    case TypeNatureMetaDataEnum::Vector:
    {
        std::vector<uint8_t>* ver = reinterpret_cast<std::vector<uint8_t>*>(objetPtr);
        const Vector& vector = type.metaData.typeNatureMetaData.metaDataType.vector;
        const ReflectedType& underLineType = Reflector::GetType(vector.type);
        try
        {
            const size_t size = _jsonFile[CONTAINER_SIZE];
            ver->resize(size* underLineType.size);

            for (size_t i = 0; i < size; i++)
            {
                const size_t offSet = i * underLineType.size;
                DeserializeType(_jsonFile[std::to_string(i)], ver->data() + offSet, underLineType.typeId);
            }
        }
        catch (...)
        {

        }

        return;
    }
        break;
    case TypeNatureMetaDataEnum::Map:
    {
      
        return;
    }
    case TypeNatureMetaDataEnum::UnordoredMap:
    {
        UnordoredByteMap& map = *reinterpret_cast<UnordoredByteMap*>(objetPtr);
        const ReflectedMap& reflectedMap = type.metaData.typeNatureMetaData.metaDataType.mapReflected;

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
            const size_t size = _jsonFile[CONTAINER_SIZE];

            std::string indexs;
            for (size_t i = 0; i < size; i++)
            {
                indexs = std::to_string(i);

                try
                {
                    DeserializeType(_jsonFile[indexs][KEY], keyBuffer.get(), keyType.typeId);
                    auto* ref = &(map.*inserFunc)(*keyBuffer.get());

                    DeserializeType(_jsonFile[indexs][VALUE], ref, valueType.typeId);
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
        return;
    }
    case TypeNatureMetaDataEnum::None:
    default:
        break;
    }


    if (type.typeFlags & TypeFlagBits::COMPOSITE)
    {
        if (type.metaData.createFunc != nullptr)
            type.metaData.createFunc(objetPtr);

        for (auto& member : type.metaData.members)
        {
            if (member.memberFlag & MemberEnumFlag::NOTSERIALIZE)
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


void Serializer::Derializing(uint8_t* _objetPtr, const fs::path& _fileToSerialize, TypeId _typeKey)
{
    std::ifstream f(_fileToSerialize.generic_string());
    json j = json::parse(f);
    f.close();
    
    const ReflectedType& type = Reflector::GetType(_typeKey);
    DeserializeType(j[type.name], _objetPtr, _typeKey);
}

#pragma endregion
