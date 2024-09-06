#include "serialize/serializer.h"

#include <RapidXML/rapidxml.hpp>

#include "RapidXML/RapidXMLSTD.hpp"

void PC_CORE::Serializer::Serializing(uint32_t _TypeKey)
{
    const ReflectedType& reflection = Reflector::GetType(_TypeKey);

    for (const Members& members : reflection.members)
    {
        SerializeMember(members);
    }
    
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

void PC_CORE::Serializer::SerializeBegin(const fs::path& _fileToSerialize)
{
    std::string err;
    currentFile = OpenXMLFile(_fileToSerialize.generic_string(), err);

    if (currentFile == nullptr)
    {
        PC_LOGERROR("Failed to open XML file "  + _fileToSerialize.generic_string() + '\n' + err);
        return;
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

