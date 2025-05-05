#include "time/core_time.hpp"

#include <GLFW/glfw3.h>

float PC_CORE::Time::GetTime()
{
    return static_cast<float>(currentTime);
}

float PC_CORE::Time::DeltaTime()
{
    return static_cast<float>(deltaTime);
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
