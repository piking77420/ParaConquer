#include "..\include\window.hpp"

#include "app.hpp"

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
    window = glfwCreateWindow(widht, height, "Vulkan window", nullptr, nullptr);
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
}

void Window::OnResize()
{
    int NewWidth = 0, NewHeight = 0;
    glfwGetFramebufferSize(window, &NewWidth, &NewHeight);
    while (NewWidth == 0 || NewHeight == 0) {
        glfwGetFramebufferSize(window, &NewWidth, &NewHeight);
        glfwWaitEvents();
    }

    widht = static_cast<uint32_t>(NewWidth);
    height = static_cast<uint32_t>(NewHeight);
}

float Window::GetAspect() const
{
    return static_cast<float>(widht) / static_cast<float>(height);
}
