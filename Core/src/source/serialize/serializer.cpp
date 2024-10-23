#include "serialize/serializer.h"

#include <RapidXML/rapidxml.hpp>
#include <any>

#include "GLFW/glfw3native.h"
#include "RapidXML/RapidXMLSTD.hpp"
#include "resources/Resource.hpp"

using namespace PC_CORE;

constexpr uint32_t XmlVersion = 1;

// bool //
constexpr const char* boolAlpha0s = "false";
constexpr const char* boolAlpha1s = "true";
constexpr bool boolAlpha0b = false;
constexpr bool boolAlpha1b = true;
//

#pragma region SerializationType


std::string DataNatureToString(DataNature _dataNature, const void* _dataPtr)
{
    switch (_dataNature)
    {
    case DataNature::UNKNOWN:
        break;
    case DataNature::BOOL:
        {
            const bool boolValue = *static_cast<const bool*>(_dataPtr);
            return boolValue ? boolAlpha1s : boolAlpha0s;
        }
    case DataNature::INT:
        {
            const int intValue = *static_cast<const int*>(_dataPtr);
            return std::to_string(intValue);
        }
    case DataNature::VEC2I:
        {
            const Tbx::Vector2i vector2I = *static_cast<const Tbx::Vector2i*>(_dataPtr);
            return "x :" + std::to_string(vector2I.x) + ", y :" + std::to_string(vector2I.y);
        }
    case DataNature::VEC3I:
        {
            const Tbx::Vector3i vector3I = *static_cast<const Tbx::Vector3i*>(_dataPtr);
            return "x :" + std::to_string(vector3I.x) + ", y :" + std::to_string(vector3I.y) + ", z :" +
                std::to_string(vector3I.z);
        }
    case DataNature::UINT:
        {
            const uint32_t uint32Value = *static_cast<const uint32_t*>(_dataPtr);
            return std::to_string(uint32Value);
        }
    case DataNature::FLOAT:
        {
            const float floatValue = *static_cast<const float*>(_dataPtr);
            return std::to_string(floatValue);
        }
    case DataNature::DOUBLE:
        {
            const double doubleValue = *static_cast<const double*>(_dataPtr);
            return std::to_string(doubleValue);
        }
    case DataNature::VEC2:
        {
            const Tbx::Vector2f vector2fValue = *static_cast<const Tbx::Vector2f*>(_dataPtr);
            return "x :" + std::to_string(vector2fValue.x) + ", y :" + std::to_string(vector2fValue.y);
        }
    case DataNature::VEC3:
        {
            const Tbx::Vector3f vector3f = *static_cast<const Tbx::Vector3f*>(_dataPtr);
            return "x :" + std::to_string(vector3f.x) + ", y :" + std::to_string(vector3f.y) + ", z :" +
                std::to_string(vector3f.z);
        }
    case DataNature::VEC4:
        {
            const Tbx::Vector4f vector4f = *static_cast<const Tbx::Vector4f*>(_dataPtr);
            return "x :" + std::to_string(vector4f.x) + ", y :" + std::to_string(vector4f.y) + ", z :" +
                std::to_string(vector4f.z) + ", w :" + std::to_string(vector4f.w);
        }
    case DataNature::QUAT:
        {
            const Tbx::Quaternionf quatf = *static_cast<const Tbx::Quaternionf*>(_dataPtr);
            return "w :" + std::to_string(quatf.real) + ", i :" + std::to_string(quatf.imaginary.x) + ", j :" +
                std::to_string(quatf.imaginary.y) + ", k :" + std::to_string(quatf.imaginary.z);
        }
    case DataNature::STRING:
        return *static_cast<const std::string*>(_dataPtr);
        break;
    }
    return "";
}
#pragma endregion

#pragma region DeSerializationType

void StringToValue(uint8_t* _dataPtr, DataNature _dataNature, const std::string& _dataString)
{
    // Container and compisite are not undle yet

    switch (_dataNature)
    {
    case DataNature::UNKNOWN:
        break;
    case DataNature::BOOL:
        *reinterpret_cast<bool*>(_dataPtr) = _dataString == boolAlpha0s ? boolAlpha1b : boolAlpha0b;
        break;
    case DataNature::INT:
        *reinterpret_cast<int*>(_dataPtr) = std::stoi(_dataString);
        break;
    case DataNature::VEC2I:
        break;
    case DataNature::VEC3I:
        break;
    case DataNature::UINT:
        *reinterpret_cast<uint32_t*>(_dataPtr) = std::stoi(_dataString);
        break;
    case DataNature::FLOAT:
        *reinterpret_cast<float*>(_dataPtr) = std::stof(_dataString);
        break;
    case DataNature::DOUBLE:
        *reinterpret_cast<double*>(_dataPtr) = std::stod(_dataString);
        break;
    case DataNature::VEC2:
        break;
    case DataNature::VEC3:
        break;
    case DataNature::VEC4:
        break;
    case DataNature::QUAT:
        break;
    case  DataNature::STRING :
        *reinterpret_cast<std::string*>(_dataPtr) = _dataString;
        break;
    case DataNature::COUNT:
        break;
    }
}

#pragma endregion

void SerializeMember(XMLDocument* _document, const uint8_t* _objetPtr, XMLElement* _currentElement,
                     const Members& _members
                     , std::string& _err)
{
    if (_members.enumFlag & NOTSERIALIZE)
        return;
    
    const uint8_t* memberPtr = _objetPtr + _members.offset;
    const ReflectedType& reflectionType = Reflector::GetType(_members.typeKey);

    if ((reflectionType.typeInfo.typeInfoFlags & TypeFlag::COMPOSITE))
    {
        XMLElement* newElement = CreateElement(_document, _members.membersName, "", _err);
        AddElementToElement(_currentElement, newElement, _err);
        
        for (auto& memberMembers : reflectionType.members)
        {
            SerializeMember(_document, memberPtr, newElement, memberMembers, _err);
        }
    }
    else if ((reflectionType.typeInfo.typeInfoFlags & TypeFlag::VECTOR))
    {
        // TODO MAKE VECTOR
    }
    else
    {
        XMLAttributte* newAttribute = CreateAttribute(_document, _members.membersName,
           DataNatureToString(reflectionType.typeInfo.dataNature, memberPtr), _err);
        AddAttributeToElement(_currentElement, newAttribute, _err);
    }
}


void PC_CORE::Serializer::Serializing(const uint8_t* objetPtr, const fs::path& _fileToSerialize, uint32_t _typeKey)
{
    std::string err;

    XMLDocument* xmlDoc = CreateXML(XmlVersion, "utf-8", err);

    if (xmlDoc == nullptr)
    {
        throw std::runtime_error("Failed to create serialize context" + err);
    }

    const ReflectedType& reflectionType = Reflector::GetType(_typeKey);
    XMLElement* root = CreateElement(xmlDoc, reflectionType.name, "", err);

    if (!AddElementToDocument(xmlDoc, root, err))
    {
        PC_LOGERROR(err);
    }

    for (auto& t : reflectionType.members)
    {
        SerializeMember(xmlDoc, objetPtr, root, t, err);
    }

    bool result = SaveXML(*xmlDoc, _fileToSerialize.generic_string());
    if (!result)
    {
        PC_LOGERROR("Failed to serialize Object " + reflectionType.name + " in " + _fileToSerialize.generic_string());
    }

    ::DisposeXMLObject(xmlDoc);
}



void DeserializeMember(uint8_t* _objetPtr, XMLElement* _currentElement,
                     const Members& _members
                     , std::string& _err)
{
    if (_members.enumFlag & NOTSERIALIZE)
       return;
    
    uint8_t* memberPtr = _objetPtr + _members.offset;
    const ReflectedType& reflectionType = Reflector::GetType(_members.typeKey);

    // Is a trivial type
    if (!(reflectionType.typeInfo.typeInfoFlags & TypeFlag::COMPOSITE))
    {
        XMLAttributte* newAttribute = _currentElement->first_attribute(_members.membersName.c_str());
        StringToValue(memberPtr, reflectionType.typeInfo.dataNature, newAttribute->value());
    }
    else
    {
        XMLElement* newCurrentElement = _currentElement->first_node(_members.membersName.c_str());
        const ReflectedType& reflectionType = Reflector::GetType(_members.typeKey);
        for (auto& member : reflectionType.members)
        {
            DeserializeMember(memberPtr, newCurrentElement, member, _err);
        }
    }
    
}

void Serializer::Derializing(uint8_t* _objetPtr, const fs::path& _fileToSerialize, uint32_t _typeKey)
{
    std::string err;
    const ReflectedType& reflectionType = Reflector::GetType(_typeKey);
    XMLFile* xmlFile = OpenXMLFile(_fileToSerialize.generic_string(), err);
    if (xmlFile == nullptr)
    {
        PC_LOGERROR("Failed to create Xml file for desrialization " + _fileToSerialize.generic_string() + err);
    }

    XMLDocument* xmlDoc = CreateXMLFromFile(xmlFile, err);
    if (xmlDoc == nullptr)
    {
        PC_LOGERROR("the file " + _fileToSerialize.generic_string() + " is not a xmlFile " + err);
    }

    XMLElement* root = FirstOrDefaultElement(xmlDoc, reflectionType.name, err);

    if (root == nullptr)
    {
        PC_LOGERROR("the file is not a " + reflectionType.name + "object " + err);
    }

    for (auto& t : reflectionType.members)
    {
        DeserializeMember(_objetPtr, root, t, err);
    }
}
