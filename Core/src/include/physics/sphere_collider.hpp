#pragma once

#include "core_header.hpp"
#include "reflection/reflector.hpp"
#include "world/component.hpp"

BEGIN_PCCORE
struct SphereCollider
{
    MAKE_COMPONENT(SphereCollider)

    bool isTrigger = false;
    
    float radius = 1.f;
};

REFLECT(SphereCollider,radius)

END_PCCORE