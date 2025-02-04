#pragma once

#include "core_header.hpp"
#include "ecs_front.h"

BEGIN_PCCORE
struct Component
{
    // DO NOT INIT THIS VALUE 
    EntityId entityId;
};

REFLECT(Component)

END_PCCORE