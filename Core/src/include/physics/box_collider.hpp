#pragma once

#include "core_header.hpp"
#include "math/matrix_transformation.hpp"
#include "reflection/reflector.hpp"

BEGIN_PCCORE

struct BoxCollider
{
    /*
    MAKE_COMPONENT(BoxCollider)
*/
    uint32_t body_Id = -1;

    bool draw = false;

    bool drawAABB = false;
    
    Tbx::Vector3f size = Tbx::Vector3f(1.f);
    Tbx::Vector3f center;
};
/*
REFLECT(BoxCollider, drawAABB)
REFLECT(BoxCollider, draw)
REFLECT(BoxCollider, size)
REFLECT(BoxCollider, center)
*/
END_PCCORE