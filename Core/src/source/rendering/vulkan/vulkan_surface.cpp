#include "rendering/vulkan/vulkan_surface.hpp"

#include <stacktrace>

#include "log.hpp"

using namespace PC_CORE;

void VulkanSurface::Init(const VkInstance& _instance, GLFWwindow* _window)
{
   const VkResult result = glfwCreateWindowSurface(_instance, _window, nullptr, &surfaceKhr);
   VK_CHECK_ERROR(result,"error while creating VkSurface");
   PC_LOG("Creating VkSurface");
   
}

void VulkanSurface::Destroy(const VkInstance& _instance)
{
    vkDestroySurfaceKHR(_instance, surfaceKhr , nullptr);
}
