#pragma once

#include "core_header.hpp"
#include "ecs/ecs_front.h"
#include "math/quaternion.hpp"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE


struct Transform : Component
{
    EntityId parentId = INVALID_ENTITY_ID;

    Tbx::Vector3f position;

    Tbx::Quaternionf rotation = Tbx::Quaternionf::Identity();

    Tbx::Vector3f scale = 1.f;

    // tmpr
    Tbx::Vector3f localPosition;

    Tbx::Vector3f localRotation;
};

REFLECT(Transform, Component)
REFLECT_MEMBER(Transform, position)
REFLECT_MEMBER(Transform, rotation)
REFLECT_MEMBER(Transform, scale)

END_PCCORE