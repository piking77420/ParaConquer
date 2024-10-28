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

        PC_CORE_API void PoolEvents();

        PC_CORE_API float GetAspect() const;

        PC_CORE_API Tbx::Vector2ui GetWindowSize() const;
    
        PC_CORE_API GLFWwindow* GetHandle();

        PC_CORE_API Window(const char* _windowName, const char* _logoPath);

        PC_CORE_API ~Window();
    private:
        Tbx::Vector2ui m_WindowSize = {1680, 1050};

        GLFWmonitor* m_Monitor = nullptr;

        GLFWwindow* m_Window = nullptr;

        const GLFWvidmode* Mode = nullptr;

        Tbx::Vector2ui oldPos;
        Tbx::Vector2ui oldSize;
        Tbx::Vector2ui monitorSize;

        bool onResize = false;

        bool FullScreen = false;

        std::string m_WindowName;
    };
END_PCCORE
