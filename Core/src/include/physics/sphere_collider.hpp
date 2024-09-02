#pragma once

#include "core_header.hpp"
#include "ecs/component.h"
#include "reflection/reflector.hpp"

BEGIN_PCCORE
struct SphereCollider : public Component
{

    bool isTrigger = false;
    
    float radius = 1.f;
};

END_PCCORE