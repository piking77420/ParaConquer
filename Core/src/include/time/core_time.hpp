#pragma once

#include "core_header.hpp"

BEGIN_PCCORE

class Time
{
public:
    static float GetTime();

    static float DeltaTime();

    static void UpdateTime();

    static void Init();

private:
    static inline double currentTime = 0.f;

    static inline double deltaTime = 0.f;

};

END_PCCORE