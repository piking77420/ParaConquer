#include "vulkan_instance.hpp"

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>

constexpr std::array<const char*, 1> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

#ifdef _DEBUG


static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << '\n';

    return VK_FALSE;
}


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void Vulkan::VulkanInstance::SetupDebugMessenger()
{
    if constexpr (!ENABLE_VALIDATION_LAYERS)
        return;

    vk::DebugUtilsMessengerCreateInfoEXT createInfo = {};
    PopulateDebugMessengerCreateInfo(createInfo);
    VK_CHECK_CALL(static_cast<vk::Result>(CreateDebugUtilsMessengerEXT(m_Instance, reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, reinterpret_cast<VkDebugUtilsMessengerEXT*>(&m_DebugMessenger))));
}

void Vulkan::VulkanInstance::PopulateDebugMessengerCreateInfo(
    vk::DebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;

    
    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
    | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
    createInfo.pfnUserCallback = DebugCallBack;
}

bool Vulkan::VulkanInstance::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    VK_CHECK_CALL(vk::enumerateInstanceLayerProperties(&layerCount, nullptr));

    std::vector<vk::LayerProperties> availableLayers(layerCount);
    VK_CHECK_CALL(vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

#endif



Vulkan::VulkanInstance::VulkanInstance(const PC_CORE::RenderInstanceCreateInfo& _renderInstanceCreateInfo) : RenderInstance(_renderInstanceCreateInfo)
{
    vk::ApplicationInfo appInfo = {};
    appInfo.sType = vk::StructureType::eApplicationInfo;
    appInfo.pApplicationName = _renderInstanceCreateInfo.appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "ParaConquer";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_MAKE_VERSION(1, 3, 8);

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
#ifdef _DEBUG
    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
#endif
    
    vk::InstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = vk::StructureType::eInstanceCreateInfo;
    instanceCreateInfo.pNext = nullptr;
    instanceCreateInfo.flags = {};
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = 0;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

#ifdef _DEBUG
    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        if (!CheckValidationLayerSupport())
            throw std::runtime_error("validation layers requested, but not available!");
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        instanceCreateInfo.enabledLayerCount = 0;
    }
#endif
    VK_CHECK_CALL(vk::createInstance(&instanceCreateInfo, nullptr, &m_Instance));

#ifdef _DEBUG
    SetupDebugMessenger();
#endif
}

Vulkan::VulkanInstance::~VulkanInstance()
{

#ifdef _DEBUG
    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }
    #endif
    
    m_Instance.destroy();
}



