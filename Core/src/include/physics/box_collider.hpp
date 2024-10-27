#pragma once

#include "core_header.hpp"
#include "ecs/ecs_front.h"
#include "math/matrix_transformation.hpp"
#include "reflection/reflector.hpp"

BEGIN_PCCORE

struct BoxCollider : public Component
{
    uint32_t body_Id = -1;

    bool draw = false;

    bool drawAABB = false;
    
    Tbx::Vector3f size = Tbx::Vector3f(1.f);
    Tbx::Vector3f center;
};
REFLECT(BoxCollider, Component)
REFLECT_MEMBER(BoxCollider, drawAABB)
REFLECT_MEMBER(BoxCollider, draw)
REFLECT_MEMBER(BoxCollider, size)
REFLECT_MEMBER(BoxCollider, center)

END_PCCORE