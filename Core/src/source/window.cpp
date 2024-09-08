#include "..\include\window.hpp"

#include <complex.h>

#include "app.hpp"
#include "imgui.h"

using namespace PC_CORE;

void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    Window* app = static_cast<Window*>(glfwGetWindowUserPointer(window));
    app->onResize = true;
}

void Window::Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);
    monitorSize = { static_cast<uint32_t>(mode->width), static_cast<uint32_t>(mode->height)}; 
    CreateWindow();
    
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
    currentWindow = this;

}

void Window::Destroy()
{
    glfwDestroyWindow(window);

    glfwTerminate();
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

void Window::PoolEvents()
{
    glfwPollEvents();
    return;
    
    if (ImGui::IsKeyReleased(ImGuiKey_F11))
    {
        FullScreen = !FullScreen;
        
        if (FullScreen)
        {
            oldSize = windowSize;
            windowSize = monitorSize;
            int x,y;
            glfwGetWindowPos(window,&x,&y);
            oldPos = {static_cast<uint32_t>(x),static_cast<uint32_t>(y)};
            
            glfwSetWindowMonitor(window, monitor, 0, 0,
                static_cast<int32_t>(windowSize.x), static_cast<int32_t>(windowSize.y), mode->refreshRate);
        }
        else
        {
            windowSize = oldSize;
            glfwSetWindowMonitor(window, NULL, static_cast<int32_t>(oldPos.x), static_cast<int32_t>(oldPos.y),
            static_cast<int32_t>(windowSize.y),static_cast<int32_t>(windowSize.y), mode->refreshRate);
        }
        
    }
}

void Window::OnResize()
{
    
    int NewWidth = 0, NewHeight = 0;
    glfwGetFramebufferSize(window, &NewWidth, &NewHeight);
    while (NewWidth == 0 || NewHeight == 0) {
        glfwGetFramebufferSize(window, &NewWidth, &NewHeight);
        glfwWaitEvents();
    }

    windowSize.x = static_cast<uint32_t>(NewWidth);
    windowSize.y = static_cast<uint32_t>(NewHeight);
}

float Window::GetAspect() const
{
    return static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
}

void Window::CreateWindow()
{
    if (FullScreen)
    {
        windowSize.x = static_cast<uint32_t>(glfwGetVideoMode(glfwGetPrimaryMonitor())->width);
        windowSize.y = static_cast<uint32_t>(glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
        window = glfwCreateWindow(static_cast<int32_t>(windowSize.x),
                                           static_cast<int32_t>(windowSize.y), App::appName,
                                            glfwGetPrimaryMonitor(), nullptr);
    }
    else
    {
        window = glfwCreateWindow(static_cast<int32_t>(windowSize.x),
                                           static_cast<int32_t>(windowSize.y), "Vulkan window", nullptr, nullptr);
    }
}
