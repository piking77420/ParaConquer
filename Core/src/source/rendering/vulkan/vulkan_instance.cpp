#include "rendering/vulkan/vulkan_instance.hpp"

#include <string>
#include <vector>

#include "app.hpp"
#include "log.hpp"
#include "rendering/vulkan/vulkan_debug_message.hpp"

using namespace PC_CORE;

void VulkanInstance::Init()
{
    std::vector<const char*> layers = 
    {
#ifndef  NDEBUG
        "VK_LAYER_KHRONOS_validation"
#endif
    };
  
    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined (_WIN32)
        "VK_KHR_win32_surface",
#endif
#if defined (__APPLE__)
        "VK_MVK_macos_surface",
#endif
#if defined (__linux__)
        "VK_KHR_xcb_surface",
#endif
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    };
    
    VkApplicationInfo AppInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "App::appName",
        .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .pEngineName = "Ogldev Vulkan Tutorials",
        .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };

    VkInstanceCreateInfo CreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &AppInfo,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    VkResult res = vkCreateInstance(&CreateInfo, NULL, &instance);
    VK_CHECK_ERROR(res, "Create instance");
    PC_LOG("Vulkan instance created");
}

void VulkanInstance::Destroy()
{
    PC_LOG("Destroy Vulkan Instance");
    vkDestroyInstance(instance, nullptr);
}
