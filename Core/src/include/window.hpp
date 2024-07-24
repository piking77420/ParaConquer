#pragma once

#include <GLFW/glfw3.h>
#include "core_header.hpp"
#include <math/toolbox_typedef.hpp>

BEGIN_PCCORE
    class Window
{
public:
    bool FullScreen = false; 

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    
    Tbx::Vector2ui windowSize = {1680,1050};

    GLFWwindow* window = nullptr;

    bool onResize = false;
    
    void Init();

    void Destroy();

    bool ShouldClose();

    void PoolEvents();

    void OnResize();

    float GetAspect() const;

    void CreateWindow();
    
    static inline Window* currentWindow;
private:
    GLFWmonitor* monitor = nullptr;
    
    const GLFWvidmode* mode = nullptr;

    Tbx::Vector2ui oldPos;
    Tbx::Vector2ui oldSize;
    Tbx::Vector2ui monitorSize;

};
END_PCCORE
