#pragma once

#include "CoreHeader.hpp"
#include "Reflection/Reflector.hpp"
#include "World/Scene.hpp"
#include "Core/MotionCoreEngine.hpp"

BEGIN_PCCORE
    class PhysicsWrapper
    {
    public:
        static constexpr float timeStep = 0.02f;

        PC_CORE_API PhysicsWrapper() = default;

        PC_CORE_API ~PhysicsWrapper() = default;

    private:
        MotionCore::MotionCoreEngine m_MotionCore;
    };


END_PCCORE
