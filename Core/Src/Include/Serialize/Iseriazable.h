#pragma once

#include <Json/Json.hpp>

#include "CoreHeader.hpp"
#include "Serializer.h"
#include "Reflection/Reflector.hpp"

BEGIN_PCCORE

class ISeriazable 
{
public:

   /*
   PC_CORE_API void Seriliaze(const fs::path& _path) const 
   {
      if (m_Type == nullptr)
      {
         PC_LOGERROR("Type was null when Seriliaze");
         return;
      }
      
      //Serializer::Serialize(m_Type->typeId, this, _path);
   }

   PC_CORE_API void DeSeriliaze(const fs::path& _path)
   {
      if (m_Type == nullptr)
      {
         PC_LOGERROR("Type was null when DeSeriliaze");
         return;
      }
      
      //Serializer::DeSerialize(m_Type->typeId, this, _path);
   }*/
   

protected:
   
};


END_PCCORE