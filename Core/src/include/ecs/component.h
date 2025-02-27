#pragma once

#include "core_header.hpp"
#include "ecs_front.h"



BEGIN_PCCORE

using ComponentTypeBit = uint32_t;

struct Component
{
    PC_FORCE_INLINE EntityId GetEntityId() const
    {
        return entityId;
    }
    
private:
    // DO NOT INIT THIS VALUE OR I WILL KILL YOU
    // THIS entityId IS SET BEFORE THE CONSTRUCTOR 
    // IN ORDER TO HAVE THE ENTITY ID AT CONTRUCTOR
    EntityId entityId;

};
template<class T>
   concept ComponentDerived = std::is_base_of_v<Component, T>;

REFLECT(Component)

END_PCCORE