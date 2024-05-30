#pragma once

#include <GLFW/glfw3.h>
#include "core_header.hpp"

BEGIN_PCCORE

class Window
{
public:
    uint32_t widht = 800;

    uint32_t height = 600;

    GLFWwindow* window = nullptr;
    
    void Init();

    void Destroy();

    bool ShouldClose();

    void PoolEvents();
};
END_PCCORE
