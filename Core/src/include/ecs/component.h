#pragma once

#include "core_header.hpp"
#include "ecs_front.h"



BEGIN_PCCORE

class EntityManager;

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

REFLECT(Component)

END_PCCORE