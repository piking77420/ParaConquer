#pragma once

#include "core_header.hpp"
#include "core/physics_engine.h"
#include "world/scene.hpp"

BEGIN_PCCORE
class PhysicsWrapper
{
public:
    static constexpr float timeStep = 0.0001f;
    
    void InitPhysicBody(Scene* _scene);

    void UpdatePhysics(float _deltatime, Scene* _scene);

    PhysicsWrapper();

    ~PhysicsWrapper();
private:
    void InitSphereCollider(Scene* _scene);
    
    MotionCore::PhysicsEngine physicsEngine;
};

END_PCCORE