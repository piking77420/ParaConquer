#pragma once

#include "component.hpp"
#include "core_header.hpp"
#include "reflection/reflector.hpp"
#include "math/quaternion.hpp"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE
class Transform
{
public:
    MAKE_COMPONENT(Transform)

    Entity parentId = NULL_ENTITY;
    
    Tbx::Vector3f position;

    Tbx::Quaternionf rotation = Tbx::Quaternionf::Identity();

    Tbx::Vector3f scale = 1.f;

    Tbx::Vector3f localPosition;

    Tbx::Vector3f localRotation;
    
}; 
REFLECT(Transform, localPosition)
REFLECT(Transform, localRotation)
REFLECT(Transform, scale)

END_PCCORE