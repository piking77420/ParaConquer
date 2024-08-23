#pragma once

#include "core_header.hpp"
#include "reflection/reflector.hpp"
#include "world/component.hpp"

BEGIN_PCCORE

struct RigidBody
{
    MAKE_COMPONENT(RigidBody);

    bool useGravity = true;

    float mass = 1.f;

    float restitutionCoef = 1.f;

    float damping = 0.90f;

    float angularDamping = 0.1f;
};

REFLECT(RigidBody, mass);
REFLECT(RigidBody, useGravity);
REFLECT(RigidBody, restitutionCoef);
REFLECT(RigidBody, damping);
REFLECT(RigidBody, angularDamping);

END_PCCORE