#pragma once

#include "core_header.hpp"
#include "ecs/component.h"
#include "reflection/reflector.hpp"

BEGIN_PCCORE

MAKE_ECS_COMPONENT(SphereCollider)

    uint32_t body_Id = -1;

    bool draw = false;

    bool drawAABB = false;

    bool isTrigger = false;
    
    float radius = 2.f;

    Tbx::Vector3f center;
};
/*
REFLECT(SphereCollider,Component)
REFLECT(SphereCollider, center)
REFLECT(SphereCollider, drawAABB)
REFLECT(SphereCollider, draw)
REFLECT(SphereCollider, radius)*/

END_PCCORE