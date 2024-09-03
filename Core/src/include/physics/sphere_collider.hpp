#pragma once

#include "core_header.hpp"
#include "ecs/component.h"
#include "reflection/reflector.hpp"

BEGIN_PCCORE

MAKE_ECS_COMPONENT(SphereCollider)

    bool isTrigger = false;
    
    float radius = 1.f;
};
REFLECT(SphereCollider,Component)

END_PCCORE