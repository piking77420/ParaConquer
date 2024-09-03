#pragma once

#include "core_header.hpp"
#include "ecs_front.h"

BEGIN_PCCORE
struct Component
{
    EntityId entityId;
};

REFLECT(Component)

END_PCCORE