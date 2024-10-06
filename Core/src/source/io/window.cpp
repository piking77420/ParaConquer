#include "io/window.hpp"

#include <GLFW/glfw3.h>

#include "rendering/render_harware_interface/RHI.hpp"


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

void Window::PoolEvents()
{
    HandleResize();

    if (glfwGetKey(m_Window, GLFW_KEY_F11) == GLFW_PRESS)
    {
        FullScreen = !FullScreen;

        if (FullScreen)
        {
            oldSize = m_WindowSize;
            m_WindowSize = monitorSize;
            int x,y;
            glfwGetWindowPos(m_Window,&x,&y);
            oldPos = {static_cast<uint32_t>(x),static_cast<uint32_t>(y)};
            
            glfwSetWindowMonitor(m_Window, m_Monitor, 0, 0,
                static_cast<int32_t>(m_WindowSize.x), static_cast<int32_t>(m_WindowSize.y), Mode->refreshRate);
        }
        else
        {
            m_WindowSize = oldSize;
            glfwSetWindowMonitor(m_Window, NULL, static_cast<int32_t>(oldPos.x), static_cast<int32_t>(oldPos.y),
            static_cast<int32_t>(m_WindowSize.y),static_cast<int32_t>(m_WindowSize.y), Mode->refreshRate);
        }
    }
}

void Window::HandleResize()
{
    if (onResize)
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_Window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(m_Window, &width, &height);
            glfwWaitEvents();
        }
        RHI::GetInstance()->WaitDevice();
        m_WindowSize = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
        RHI::GetInstance()->RecreateSwapChain(m_Window, m_WindowSize.x, m_WindowSize.y);
        onResize = false;
    }
}

float Window::GetAspect() const
{
    return static_cast<float>(m_WindowSize.x) / static_cast<float>(m_WindowSize.y);
}

Tbx::Vector2ui Window::GetWindowSize() const
{
    return m_WindowSize;
}

GLFWwindow* Window::GetHandle()
{
    return m_Window;
}

Window::Window(const char* _windowName) : m_WindowName(_windowName)
{
    m_Monitor = glfwGetPrimaryMonitor();
    Mode = glfwGetVideoMode(m_Monitor);
    monitorSize = { static_cast<uint32_t>(Mode->width), static_cast<uint32_t>(Mode->height)};
    
    if (FullScreen)
    {
        m_WindowSize.x = static_cast<uint32_t>(glfwGetVideoMode(glfwGetPrimaryMonitor())->width);
        m_WindowSize.y = static_cast<uint32_t>(glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
        m_Window = glfwCreateWindow(static_cast<int32_t>(m_WindowSize.x),
                                           static_cast<int32_t>(m_WindowSize.y), m_WindowName.c_str(),
                                            glfwGetPrimaryMonitor(), nullptr);
    }
    else
    {
        m_Window = glfwCreateWindow(static_cast<int32_t>(m_WindowSize.x),
                                           static_cast<int32_t>(m_WindowSize.y), m_WindowName.c_str(), nullptr, nullptr);
    }

    glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);
    glfwSetWindowUserPointer(m_Window, this);
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
}
