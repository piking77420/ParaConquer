#pragma once

#include "core_header.hpp"
#include "ecs/ecs_front.h"
#include "reflection/reflector.hpp"

BEGIN_PCCORE

struct RigidBody : public Component
{

    bool useGravity = true;

    float mass = 1.f;

    float restitutionCoef = 1.f;

    float damping = 0.90f;

    float angularDamping = 0.1f;
};


REFLECT(RigidBody, Component);
REFLECT_MEMBER(RigidBody, useGravity);
REFLECT_MEMBER(RigidBody, restitutionCoef);
REFLECT_MEMBER(RigidBody, damping);
REFLECT_MEMBER(RigidBody, angularDamping);

END_PCCORE