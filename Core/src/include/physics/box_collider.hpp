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
    
    Tbx::Vector3f extend = Tbx::Vector3f(0.5f, 0.5f, 0.5f);
    Tbx::Vector3f center;
};
REFLECT(BoxCollider, Component)
REFLECT_MEMBER(BoxCollider, drawAABB)
REFLECT_MEMBER(BoxCollider, draw)
REFLECT_MEMBER(BoxCollider, extend)
REFLECT_MEMBER(BoxCollider, center)

END_PCCORE