#pragma once

#include <GLFW/glfw3.h>
#include "core_header.hpp"

BEGIN_PCCORE

class Window
{
public:

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    
    uint32_t widht = 800;

    uint32_t height = 600;

    GLFWwindow* window = nullptr;

    bool onResize = false;
    
    void Init();

    void Destroy();

    bool ShouldClose();

    void PoolEvents();

    void OnResize();

    static inline Window* currentWindow;
};
END_PCCORE
