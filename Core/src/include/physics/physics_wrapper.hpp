﻿#pragma once

#include "core_header.hpp"
#include "reflection/reflector.hpp"
#include "world/scene.hpp"
#include "core/motion_core_engine.hpp"

BEGIN_PCCORE
class PhysicsWrapper
{
public:
    static constexpr float timeStep = 0.02f;
   
    PC_CORE_API PhysicsWrapper() = default;

    PC_CORE_API ~PhysicsWrapper() = default;
    
private:
    MotionCoreEngine m_MotionCore;
    
};





END_PCCORE