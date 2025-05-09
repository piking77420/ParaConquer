﻿#pragma once

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

        bool resizeDirty = false;
    
        PC_CORE_API Window& operator=(Window&& _other) noexcept;

        PC_CORE_API void HandleResize();

        PC_CORE_API bool ShouldClose();

        PC_CORE_API void PoolEvents();

        PC_CORE_API float GetAspect() const;

        PC_CORE_API Tbx::Vector2ui GetWindowSize() const;
    
        PC_CORE_API GLFWwindow* GetHandle();
    
        PC_CORE_API Window(const char* _windowName, const char* _logoPath);

        PC_CORE_API Window() = default;

        PC_CORE_API ~Window();

    private:
        GLFWwindow* m_Window = nullptr;
    
        Tbx::Vector2ui m_WindowSize = {1680, 1050};

        GLFWmonitor* m_Monitor = nullptr;
    
        const GLFWvidmode* Mode = nullptr;

        Tbx::Vector2ui oldPos;
        Tbx::Vector2ui oldSize;
        Tbx::Vector2ui monitorSize;


        bool FullScreen = false;

        std::string m_WindowName;
    };
END_PCCORE
