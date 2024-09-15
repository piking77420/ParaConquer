#include "io/window.hpp"

#include <GLFW/glfw3.h>


using namespace PC_CORE;

void Window::FramebufferResizeCallback(GLFWwindow* _window, int width, int height)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(_window));
    window->onResize = true;
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

void Window::Update()
{
    
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
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
    HandleResize();

}

void Window::HandleResize()
{
    if (onResize)
    {
        int NewWidth = 0, NewHeight = 0;
        glfwGetFramebufferSize(window, &NewWidth, &NewHeight);
        while (NewWidth == 0 || NewHeight == 0) {
            glfwGetFramebufferSize(window, &NewWidth, &NewHeight);
            glfwWaitEvents();
        }
        windowSize.x = static_cast<uint32_t>(NewWidth);
        windowSize.y = static_cast<uint32_t>(NewHeight);
        onResize = false;
    }
}

float Window::GetAspect() const
{
    return static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
}

Window::Window(const char* _windowName) : m_WindowName(_windowName)
{
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);
    monitorSize = { static_cast<uint32_t>(mode->width), static_cast<uint32_t>(mode->height)};
    
    if (FullScreen)
    {
        windowSize.x = static_cast<uint32_t>(glfwGetVideoMode(glfwGetPrimaryMonitor())->width);
        windowSize.y = static_cast<uint32_t>(glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
        window = glfwCreateWindow(static_cast<int32_t>(windowSize.x),
                                           static_cast<int32_t>(windowSize.y), m_WindowName.c_str(),
                                            glfwGetPrimaryMonitor(), nullptr);
    }
    else
    {
        window = glfwCreateWindow(static_cast<int32_t>(windowSize.x),
                                           static_cast<int32_t>(windowSize.y), m_WindowName.c_str(), nullptr, nullptr);
    }

    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
    glfwSetWindowUserPointer(window, this);
}

Window::~Window()
{
    glfwDestroyWindow(window);
}
