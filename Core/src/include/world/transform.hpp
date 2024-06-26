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
    
    Vector3f position; 

    Quaternionf rotation = Quaternionf::Identity();
    
    Vector3f scale = 1.f;
    
    Vector3f localPosition;

    Vector3f localRotation;
    
}; 
REFLECT(Transform, localPosition)
REFLECT(Transform, localRotation)
REFLECT(Transform, scale)

END_PCCORE