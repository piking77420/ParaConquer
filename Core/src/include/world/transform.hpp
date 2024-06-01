﻿#pragma once

#include "component.hpp"
#include "core_header.hpp"
#include "math/quaternion.hpp"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE
class Transform
{
public:
    MAKE_COMPONENT(Transform)

    Entity parentId = NULL_ENTITY;
    
    Vector3f position;

    Quaterionf rotation = Quaterionf::Identity();
    
    Vector3f scale = 1.f;
    
private:
    Vector3f m_LocalPosition;

    Quaterionf m_LocalRotation = Quaterionf::Identity();

    
};

END_PCCORE