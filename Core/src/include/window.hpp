#pragma once

#include <GLFW/glfw3.h>
#include "core_header.hpp"
#include <math/toolbox_typedef.hpp>

BEGIN_PCCORE
    class Window
{
public:
    bool FullScreen = false; 

    PC_CORE_API static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    
    Tbx::Vector2ui windowSize = {1680,1050};

    GLFWwindow* window = nullptr;

    bool onResize = false;
    
    PC_CORE_API void Init();

    PC_CORE_API void Destroy();

    PC_CORE_API bool ShouldClose();

    PC_CORE_API void PoolEvents();

    PC_CORE_API void OnResize();

    PC_CORE_API float GetAspect() const;

    PC_CORE_API void CreateWindow();
    
    static inline Window* currentWindow;
private:
    GLFWmonitor* monitor = nullptr;
    
    const GLFWvidmode* mode = nullptr;

    Tbx::Vector2ui oldPos;
    Tbx::Vector2ui oldSize;
    Tbx::Vector2ui monitorSize;

};
END_PCCORE
