#include "serialize/serializer.h"


#include <any>

#include "GLFW/glfw3native.h"
#include "resources/resource.hpp"
#include "resources/resource_manager.hpp"

using namespace PC_CORE;

// bool //
constexpr const char* boolAlpha0s = "false";
constexpr const char* boolAlpha1s = "true";
constexpr bool boolAlpha0b = false;
constexpr bool boolAlpha1b = true;
//

#define CONTAINER_SIZE "size"

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

            if (resourceSPtr != nullptr)
            {
                _jsonFile["Guid"] = std::string(resourceSPtr->guid);
            }
            else
            {
                _jsonFile["Guid"] = resourceSPtr->guid.Empty();
            }
        }
        return;
    }
    case TypeNatureMetaDataEnum::ResourceHandle : 
    {
        auto& pointedType = Reflector::GetType(type.metaData.typeNatureMetaData.metaDataType.resourceHandleType.type);
        if (Reflector::IsBaseOf<Resource>(pointedType))
        {
            const std::shared_ptr<Resource>* rsPtr = reinterpret_cast<const std::shared_ptr<Resource>*>(objetPtr);
            
            SerializeType(_jsonFile, reinterpret_cast<const uint8_t*>(rsPtr->get()), pointedType.typeId);

        }


        return;
    }
    case TypeNatureMetaDataEnum::String:
    {
        const RelfectedString& rs = type.metaData.typeNatureMetaData.metaDataType.relfectedString;
        const TypeId& underLineType = rs.type;

        if (rs.type == Reflector::GetTypeKey<char>())
        {
            const std::string* s = reinterpret_cast<const std::string*>(objetPtr);
            _jsonFile[CONTAINER_SIZE] = s->size();
            _jsonFile["string"] = s->c_str();
        }
        else if(rs.type == Reflector::GetTypeKey<wchar_t>())
        {
            const std::wstring* s = reinterpret_cast<const std::wstring*>(objetPtr);
            _jsonFile[CONTAINER_SIZE] = s->size();
            _jsonFile["string"] = s->c_str();
        }
        else
        {
            assert(false);
        }
        return;
    }
    case TypeNatureMetaDataEnum::Array:
    {

        const Array& arr = type.metaData.typeNatureMetaData.metaDataType.array;
        const ReflectedType& underLineType = Reflector::GetType(arr.type);
        const size_t typeCount = arr.size / underLineType.size;

        _jsonFile[CONTAINER_SIZE] = typeCount;

        for (size_t i = 0; i < typeCount; i++)
        {
            const size_t offSet = i * underLineType.size;
            SerializeType(_jsonFile[std::to_string(i)], objetPtr + offSet, underLineType.typeId);
        }

        return;
    }
    case TypeNatureMetaDataEnum::Vector:
    {
        const std::vector<uint8_t>* ver = reinterpret_cast<const std::vector<uint8_t>*>(objetPtr);
        const ReflectedType& underLineType = Reflector::GetType(type.metaData.typeNatureMetaData.metaDataType.vector.type);
        const size_t typeCount = ver->size() / underLineType.size;

        _jsonFile[CONTAINER_SIZE] = typeCount;

        for (size_t i = 0; i < typeCount; i++)
        {
            const size_t offSet = i * underLineType.size;
            SerializeType(_jsonFile[std::to_string(i)], ver->data() + offSet, underLineType.typeId);
        }

        return;
    }
    return;
    case TypeNatureMetaDataEnum::Map:
        break;
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
        assert(false);
    }
    else if (Reflector::IsTypeIdIs<uint64_t>(id))
    {
        assert(false);
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
    const json& memberJson = _jsonFile[type.name][member.membersName];

    DeserializeType(memberJson, objetPtr, member.typeKey);
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
        return;
    }
    case TypeNatureMetaDataEnum::ResourceHandle:
    {
        auto& pointedType = Reflector::GetType(type.metaData.typeNatureMetaData.metaDataType.resourceHandleType.type);
        if (Reflector::IsBaseOf<Resource>(pointedType))
        {
            std::shared_ptr<Resource>* rsPtr = reinterpret_cast<std::shared_ptr<Resource>*>(objetPtr);
            std::shared_ptr<uint8_t[]> ptr = std::make_shared<uint8_t[]>(pointedType.size);
            
            // use contructor to init value and vtable
            pointedType.metaData.createFunc(ptr.get());

            DeserializeType(_jsonFile, reinterpret_cast<uint8_t*>(rsPtr->get()), pointedType.typeId);

        }


        return;
    }
    case TypeNatureMetaDataEnum::String:
    {
        const RelfectedString& rs = type.metaData.typeNatureMetaData.metaDataType.relfectedString;
        const TypeId& underLineType = rs.type;

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
        return;
    }
    case TypeNatureMetaDataEnum::Array:
        {
            const Array& arr = type.metaData.typeNatureMetaData.metaDataType.array;
            const ReflectedType& underLineType = Reflector::GetType(arr.type);
            const size_t typeCount = arr.size / underLineType.size;

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

            return;
        }
        break;
    case TypeNatureMetaDataEnum::Vector:
    {
        std::vector<uint8_t>* ver = reinterpret_cast<std::vector<uint8_t>*>(objetPtr);
        const Vector& vector = type.metaData.typeNatureMetaData.metaDataType.vector;
        const ReflectedType& underLineType = Reflector::GetType(vector.type);

        const size_t size = _jsonFile[CONTAINER_SIZE];
        ver->resize(size * underLineType.size);
   
        for (size_t i = 0; i < size; i++)
        {
            const size_t offSet = i * underLineType.size;
            DeserializeType(_jsonFile[std::to_string(i)], ver->data() + offSet, underLineType.typeId);
        }

        return;
    }
        break;
    case TypeNatureMetaDataEnum::Map:
        break;
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
