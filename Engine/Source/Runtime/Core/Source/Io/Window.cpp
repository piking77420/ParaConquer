#include "io/window.hpp"

#include <Glfw/Glfw3.h>
#include <PerfRegion.hpp>
#include <Io/FileLoader.hpp>
#include <Log.hpp>

using namespace PC_CORE;

void Window::FramebufferResizeCallback(GLFWwindow* _window, int width, int height)
{
    auto window = static_cast<Window*>(glfwGetWindowUserPointer(_window));
    window->resizeDirty = true;
}

Window& Window::operator=(Window&& _other) noexcept
{
    m_WindowSize = _other.m_WindowSize;
    _other.m_WindowSize = {};

    m_Window = _other.m_Window;
    _other.m_Window = nullptr;

    Mode = _other.Mode;
    _other.Mode = nullptr;

    m_Monitor = _other.m_Monitor;
    _other.m_Monitor = nullptr;

    oldPos = _other.oldPos;
    _other.oldPos = {};

    oldSize = _other.oldSize;
    _other.oldSize = {};

    monitorSize = _other.monitorSize;
    _other.monitorSize = {};

    resizeDirty = _other.resizeDirty;
    _other.resizeDirty = false;

    FullScreen = _other.FullScreen;
    _other.FullScreen = false;

    m_WindowName = std::move(_other.m_WindowName);
    glfwSetWindowUserPointer(m_Window, this);

    return *this;
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(m_Window);
}

void Window::PoolEvents()
{
    if (glfwGetKey(m_Window, GLFW_KEY_F11) == GLFW_PRESS)
    {
        FullScreen = !FullScreen;

        if (FullScreen)
        {
            oldSize = m_WindowSize;
            m_WindowSize = monitorSize;
            int x, y;
            glfwGetWindowPos(m_Window, &x, &y);
            oldPos = {static_cast<uint32_t>(x), static_cast<uint32_t>(y)};

            glfwSetWindowMonitor(m_Window, m_Monitor, 0, 0,
                                 static_cast<int32_t>(m_WindowSize.x), static_cast<int32_t>(m_WindowSize.y),
                                 Mode->refreshRate);
        }
        else
        {
            m_WindowSize = oldSize;
            glfwSetWindowMonitor(m_Window, nullptr, static_cast<int32_t>(oldPos.x), static_cast<int32_t>(oldPos.y),
                                 static_cast<int32_t>(m_WindowSize.y), static_cast<int32_t>(m_WindowSize.y),
                                 Mode->refreshRate);
        }
        resizeDirty = true;
    }
    HandleResize();
}

void Window::HandleResize()
{
    if (resizeDirty)
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_Window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(m_Window, &width, &height);
            glfwWaitEvents();
        }
        m_WindowSize = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
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

Tbx::Vector2d Window::GetCursorPos() const
{
    Tbx::Vector2d out;
    glfwGetCursorPos(m_Window, &out.x, &out.y);
    return out;
}


GLFWwindow* Window::GetHandle()
{
    return m_Window;
}

void Window::HideCursor(bool _hide)
{
    glfwSetInputMode(m_Window, GLFW_CURSOR, _hide ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Window::SetCursorPos(Tbx::Vector2f _pos) const
{
    glfwSetCursorPos(m_Window, _pos.x, _pos.y);
}

void Window::SetIcon(const char* _iconPath)
{
    if (_iconPath == nullptr || *_iconPath == '\0')
    {
        PC_LOGERROR("Icon path is null or empty.");
        return;
    }

    int x, y;
    auto channel = RhiChannel::Default;
    uint8_t* rawData = FileLoader::LoadImage(_iconPath, &x, &y, &channel, RhiChannel::Default, false);

    if (!rawData || x <= 0 || y <= 0)
    {
        PC_LOGERROR("Failed to load icon image file '{}'", _iconPath);
        return;
    }

    auto deleter = [](uint8_t* p) {
        FileLoader::FreeData(p);
        };

    std::unique_ptr<uint8_t[], decltype(deleter)> data(rawData, deleter);

    GLFWimage image;
    image.pixels = data.get();
    image.width = x;
    image.height = y;

    glfwSetWindowIcon(m_Window, 1, &image);
}

Window::Window(const char* _windowName) : m_WindowName(_windowName)
{
    PERF_REGION_SCOPED;
    m_Monitor = glfwGetPrimaryMonitor();
    Mode = glfwGetVideoMode(m_Monitor);
    monitorSize = {static_cast<uint32_t>(Mode->width), static_cast<uint32_t>(Mode->height)};

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
    if (m_Window != nullptr)
        glfwDestroyWindow(m_Window);
}
