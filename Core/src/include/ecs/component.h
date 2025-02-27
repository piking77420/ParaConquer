#pragma once

#include "core_header.hpp"
#include "ecs_front.h"



BEGIN_PCCORE

class EntityManager;

using ComponentTypeBit = uint32_t;

struct Component
{
    PC_FORCE_INLINE EntityId GetEntityId() const
    {
        return entityId;
    }
    
private:
    // DO NOT INIT THIS VALUE 
    EntityId entityId;

    friend EntityManager;
};
template<class T>
   concept ComponentDerived = std::is_base_of_v<Component, T>;

REFLECT(Component)

END_PCCORE