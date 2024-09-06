#include "serialize/serializer.h"

#include <RapidXML/rapidxml.hpp>

#include "RapidXML/RapidXMLSTD.hpp"
#include "resources/resource.hpp"

struct SerializeContex
{
    XMLDocument* xmlDoc;
    XMLElement* root;
};

struct DeserializeContex
{
    
};

constexpr uint32_t XmlVersion = 1;


void PC_CORE::Serializer::Serializing(const fs::path& _fileToSerialize, uint32_t _typeKey)
{
    std::string err;
    
    SerializeContex serializeContex;
    serializeContex.xmlDoc = CreateXML(XmlVersion, "utf-8", err);
    
    if (serializeContex.xmlDoc == nullptr)
    {
        throw std::runtime_error("Failed to create serialize context" + err) ;
    }

   // serializeContex.root = CreateElement()
    
}

void PC_CORE::Serializer::SerializeMember(const Members& _members)
{
    const ReflectedType& MemberReflection = Reflector::GetType(_members.typeKey);
    switch (_members.dataNature)
    {
    case DataNature::UNKNOW:
        break;
    case DataNature::BOOL:
        break;
    case DataNature::INT:
        break;
    case DataNature::VEC2I:
        break;
    case DataNature::VEC3I:
        break;
    case DataNature::UINT:
        break;
    case DataNature::FLOAT:
        break;
    case DataNature::DOUBLE:
        break;
    case DataNature::VEC2:
        break;
    case DataNature::VEC3:
        break;
    case DataNature::VEC4:
        break;
    case DataNature::QUAT:
        break;
    case DataNature::COMPOSITE:
         
        for (const Members& members : MemberReflection.members)
        {
            SerializeMember(_members);
        }
        break;
    case DataNature::CONTAINER:
        break;
    case DataNature::COUT:
        break;
    default: ;
    }
}

void PC_CORE::Serializer::SerializeEnd()
{
   bool value = ::DisposeXMLFile(static_cast<XMLFile*>(currentFile));

    if (!value)
    {
        PC_LOGERROR("Failed to Dispose XML file ");
    }
}

