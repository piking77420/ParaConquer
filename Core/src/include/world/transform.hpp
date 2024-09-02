#pragma once

#include "core_header.hpp"
#include "ecs/ecs_front.h"
#include "reflection/reflector.hpp"
#include "math/quaternion.hpp"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE


MAKE_ECS_COMPONENT(Transform)

    EntityId parentId = INVALID_ENTITY_ID;
    
    Tbx::Vector3f position;

    Tbx::Quaternionf rotation = Tbx::Quaternionf::Identity();

    Tbx::Vector3f scale = 1.f;

    Tbx::Vector3f localPosition;

    Tbx::Vector3f localRotation;
private:
    int x = 0; 
};
REFLECT_MEMBER(Transform, localPosition)
REFLECT_MEMBER(Transform, localRotation)
REFLECT_MEMBER(Transform, scale)

END_PCCORE