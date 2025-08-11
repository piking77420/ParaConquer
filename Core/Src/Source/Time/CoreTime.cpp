#include "Time/CoreTime.hpp"

#include <GLFW/glfw3.h>

double PC_CORE::Time::GetTime()
{
    return currentTime;
}

double PC_CORE::Time::DeltaTime()
{
    return deltaTime;
}

void PC_CORE::Time::UpdateTime()
{
    
    const double currentFrame = glfwGetTime();
    deltaTime = currentFrame - currentTime;
    currentTime = currentFrame;
}

void PC_CORE::Time::Init()
{
    currentTime = glfwGetTime();
}
