#pragma once

#include <GLFW/glfw3.h>

#include "core_header.hpp"
#include <math/toolbox_typedef.hpp>

struct GLFWwindow;
struct GLFWmonitor;
struct GLFWvidmode;

BEGIN_PCCORE
    class Window
    {
    public:
        PC_CORE_API static void FramebufferResizeCallback(GLFWwindow* _window, int width, int height);

        PC_CORE_API void HandleResize();

        PC_CORE_API bool ShouldClose();

        PC_CORE_API void Update();

        PC_CORE_API float GetAspect() const;

        PC_CORE_API Window(const char* _windowName);

        PC_CORE_API ~Window();
    
    private:
        Tbx::Vector2ui windowSize = {1680, 1050};

        GLFWmonitor* monitor = nullptr;

        GLFWwindow* window = nullptr;

        const GLFWvidmode* mode = nullptr;

        Tbx::Vector2ui oldPos;
        Tbx::Vector2ui oldSize;
        Tbx::Vector2ui monitorSize;

        bool onResize = false;

        bool FullScreen = false;

        std::string m_WindowName;
    };
END_PCCORE
