#pragma once

#include "core_header.hpp"

BEGIN_PCCORE

class Time
{
public:
    PC_CORE_API static float GetTime();

    PC_CORE_API static float DeltaTime();

    PC_CORE_API static void UpdateTime();

    PC_CORE_API static void Init();

private:
    static inline double currentTime = 0.f;

    static inline double deltaTime = 0.f;

};

END_PCCORE