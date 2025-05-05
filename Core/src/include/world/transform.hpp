#pragma once

#include "core_header.hpp"
#include "ecs/ecs_front.h"
#include "math/quaternion.hpp"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE


struct Rotation
{
    Tbx::Vector3d eulerAngles;
    Tbx::Quaterniond quaternion = Tbx::Quaterniond::Identity();
};

REFLECT(Rotation)
REFLECT_MEMBER(Rotation, eulerAngles)
REFLECT_MEMBER(Rotation, quaternion)

struct Transform : Component
{
    EntityId parentId = INVALID_ENTITY_ID;

    Tbx::Vector3d position;

    Rotation rotation;
    
    Tbx::Vector3d scale = Tbx::Vector3d::UnitOne();

    Tbx::Vector3d localPosition;
    
    
    // TO HANDLE MATRIX TO AVOID RECALCULATION
    
};
// TODO PUT THE REFLECT IN THE CLASS 


REFLECT(Transform, Component)
REFLECT_MEMBER(Transform, position)
REFLECT_MEMBER(Transform, rotation)
REFLECT_MEMBER(Transform, scale)

END_PCCORE