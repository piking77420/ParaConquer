#pragma once

#include "core_header.hpp"
#include "ecs/component.h"
#include "reflection/reflector.hpp"

BEGIN_PCCORE

struct SphereCollider : public Component
{

    uint32_t body_Id = -1;

    bool draw = false;

    bool drawAABB = false;

    bool isTrigger = false;
    
    float radius = 2.f;

    Tbx::Vector3f center;
};

REFLECT(SphereCollider,Component)
REFLECT_MEMBER(SphereCollider, center)
REFLECT_MEMBER(SphereCollider, drawAABB)
REFLECT_MEMBER(SphereCollider, draw)
REFLECT_MEMBER(SphereCollider, radius)

END_PCCORE