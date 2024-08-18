#pragma once

#include "core_header.hpp"
#include "reflection/reflector.hpp"
#include "world/component.hpp"

BEGIN_PCCORE
struct SphereCollider
{
    MAKE_COMPONENT(SphereCollider)

    uint32_t body_Id = -1;

    bool draw = false;

    bool isTrigger = false;
    
    float radius = 2.f;
};

REFLECT(SphereCollider, draw)
REFLECT(SphereCollider, radius)

END_PCCORE