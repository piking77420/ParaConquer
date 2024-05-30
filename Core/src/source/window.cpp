#include "..\include\window.hpp"

using namespace PC_CORE;

void Window::Init()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(widht, height, "Vulkan window", nullptr, nullptr);

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
