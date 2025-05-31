#pragma once

#include "core_header.hpp"
#include "reflection/reflector.hpp"

BEGIN_PCCORE

class ISeriazable : public DynamicReflectable
{
public:
   PC_CORE_API void QueryType() override = 0;
   
   PC_CORE_API ISeriazable() = default  ;

   PC_CORE_API virtual ~ISeriazable() = default;
protected:
};


END_PCCORE