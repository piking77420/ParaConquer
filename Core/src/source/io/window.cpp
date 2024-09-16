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
    return glfwWindowShouldClose(m_Window);
}

void Window::Update()
{
    
    if (glfwGetKey(m_Window, GLFW_KEY_F11) == GLFW_PRESS)
    {
        FullScreen = !FullScreen;

        if (FullScreen)
        {
            oldSize = windowSize;
            windowSize = monitorSize;
            int x,y;
            glfwGetWindowPos(m_Window,&x,&y);
            oldPos = {static_cast<uint32_t>(x),static_cast<uint32_t>(y)};
            
            glfwSetWindowMonitor(m_Window, monitor, 0, 0,
                static_cast<int32_t>(windowSize.x), static_cast<int32_t>(windowSize.y), mode->refreshRate);
        }
        else
        {
            windowSize = oldSize;
            glfwSetWindowMonitor(m_Window, NULL, static_cast<int32_t>(oldPos.x), static_cast<int32_t>(oldPos.y),
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
        glfwGetFramebufferSize(m_Window, &NewWidth, &NewHeight);
        while (NewWidth == 0 || NewHeight == 0) {
            glfwGetFramebufferSize(m_Window, &NewWidth, &NewHeight);
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

GLFWwindow* Window::GetHandle()
{
    return m_Window;
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
        m_Window = glfwCreateWindow(static_cast<int32_t>(windowSize.x),
                                           static_cast<int32_t>(windowSize.y), m_WindowName.c_str(),
                                            glfwGetPrimaryMonitor(), nullptr);
    }
    else
    {
        m_Window = glfwCreateWindow(static_cast<int32_t>(windowSize.x),
                                           static_cast<int32_t>(windowSize.y), m_WindowName.c_str(), nullptr, nullptr);
    }

    glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);
    glfwSetWindowUserPointer(m_Window, this);
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
}
