#pragma once

#include "core_header.hpp"
#include "ecs_front.h"

BEGIN_PCCORE
struct Component
{
    EntityId entityId = INVALID_ENTITY_ID;
};

REFLECT(Component)

END_PCCORE