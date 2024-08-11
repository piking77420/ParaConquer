#include "time/core_time.hpp"

#include "GLFW/glfw3.h"

using namespace PC_CORE;

float Time::GetTime()
{
    return static_cast<float>(currentTime);
}

float Time::DeltaTime()
{
    return static_cast<float>(deltaTime);
}

void Time::UpdateTime()
{
    const double currentFrame  = glfwGetTime();
    deltaTime = currentFrame - currentTime;
    currentTime = currentFrame;
}
