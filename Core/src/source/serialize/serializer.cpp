#include "serialize/serializer.h"

#include <Json/json.hpp>
#include <fstream>

using json = nlohmann::json;

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


#pragma region SerializationType
void TypeToString(json& outj,TypeId id, const uint8_t* objetPtr)
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

 
#pragma endregion

#pragma region DeSerializationType

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

#pragma endregion


void SerializeMember(json& _jsonFile, const Members& member, const uint8_t* objetPtr);
void SerializeType(json& _jsonFile, const uint8_t* objetPtr, TypeId _typeKey);


void SerializeMember(json& _jsonFile, const Members& member, const uint8_t* objetPtr)
{
    if (member.memberFlag & MemberEnumFlag::NOTSERIALIZE)
        return;

    auto& type = PC_CORE::Reflector::GetType(member.typeKey);
    json& jmember = _jsonFile[type.name][member.membersName];

    if (type.typeFlags & TypeFlagBits::COMPOSITE)
    {
        SerializeType(jmember, objetPtr, member.typeKey);

    }
    else if (type.typeFlags & PC_CORE::TypeFlagBits::PTR)
    {
        uint64_t ptr = (uint64_t)objetPtr;
        auto& typeRef = Reflector::GetType(type.metaData.metaDataType);

        if (Reflector::IsBaseOf<Resource>(typeRef))
        {
            const PC_CORE::Resource** doublePtr = reinterpret_cast<const PC_CORE::Resource**>(ptr);
            const PC_CORE::Resource* resource = *doublePtr;


            if (resource != nullptr)
            {
                jmember["Guid"] = std::string(resource->guid);
            }
            else
            {
                jmember["Guid"] = resource->guid.Empty();
            }
        }
    }
    else
    {
        TypeToString(jmember, member.typeKey, objetPtr);
    }
}



void SerializeType(json& _jsonFile ,const uint8_t* objetPtr, TypeId _typeKey)
{
    const ReflectedType& type = Reflector::GetType(_typeKey);

    for (auto& member : type.metaData.members)
    {
        const uint8_t* ptr = objetPtr + member.offset;

        SerializeMember(_jsonFile[type.name], member, ptr);
    }
}


void PC_CORE::Serializer::Serializing(const uint8_t* objetPtr, const fs::path& _fileToSerialize, TypeId _typeKey)
{
    std::ofstream myfile(_fileToSerialize.generic_string());

    if (!myfile.is_open()) 
    {
        return;
    }
    json j;
    SerializeType(j, objetPtr, _typeKey);

    myfile << j.dump(4);
    myfile.close();
}

void DeSerializeMember(const json& _jsonFile, const Members& member, uint8_t* objetPtr);
void DeserializeType(const json& _jsonFile, uint8_t* objetPtr, TypeId _typeKey);


void DeSerializeMember(const json& _jsonFile, const Members& member, uint8_t* objetPtr)
{
    if (member.memberFlag & MemberEnumFlag::NOTSERIALIZE)
        return;
   

    auto& type = PC_CORE::Reflector::GetType(member.typeKey);
    const json& jmember = _jsonFile[type.name][member.membersName];

    if (type.typeFlags & TypeFlagBits::COMPOSITE)
    {
        DeserializeType(jmember, objetPtr, member.typeKey);
    }
    else if (type.typeFlags & PC_CORE::TypeFlagBits::PTR)
    {
        uint64_t ptr = (uint64_t)objetPtr;

        auto& typeRef = Reflector::GetType(type.metaData.metaDataType);
        if (Reflector::IsBaseOf<Resource>(typeRef))
        {
            const PC_CORE::Resource** doublePtr = reinterpret_cast<const PC_CORE::Resource**>(ptr);
            const PC_CORE::Resource* resource = *doublePtr;

            Guid g = Guid::FromString(jmember["Guid"].get < std::string >().c_str());

            if (g == Guid::Empty())
            {
                *doublePtr = nullptr;
            }
            else
            {
                std::shared_ptr<Resource> r = PC_CORE::ResourceManager::GetByGuid(g);
                if (r == nullptr)
                {
                    PC_LOGERROR("There is no matching guiid ");
                }
                *doublePtr = r.get();
            }

        }
    }
    else
    {
        TypeFromString(jmember, member.typeKey, objetPtr);
    }
}


void DeserializeType(const json& _jsonFile, uint8_t* objetPtr, TypeId _typeKey)
{
    const ReflectedType& type = Reflector::GetType(_typeKey);

    int i = 0;
    for (auto& member : type.metaData.members)
    {
        uint8_t* ptr = objetPtr + type.metaData.members[i].offset;

        DeSerializeMember(_jsonFile[type.name], type.metaData.members[i], ptr);
        i++;
    }
}


void Serializer::Derializing(uint8_t* _objetPtr, const fs::path& _fileToSerialize, TypeId _typeKey)
{
    std::ifstream f(_fileToSerialize.generic_string());
    json j = json::parse(f);
    f.close();
    
    DeserializeType(j, _objetPtr, _typeKey);
}

