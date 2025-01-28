#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "vulkan_instance.hpp"

#include <windows.h>
#include <memory>


#ifdef _DEBUG


static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
    vk::DebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
    vk::DebugUtilsMessageTypeFlagsEXT              messageTypes,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
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



Vulkan::VulkanInstance::VulkanInstance(const PC_CORE::RenderInstanceCreateInfo& _renderInstanceCreateInfo, GLFWwindow* _window) : RenderInstance(_renderInstanceCreateInfo)
{
    vk::ApplicationInfo appInfo = {};
    appInfo.sType = vk::StructureType::eApplicationInfo;
    appInfo.pApplicationName = _renderInstanceCreateInfo.appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "ParaConquer";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_4;

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

    InitSurface(_window);
}



void Vulkan::VulkanInstance::InitSurface(GLFWwindow* _window)
{
    if (_window == nullptr) {
        throw std::invalid_argument("Window handle is null");
    }

    vk::Win32SurfaceCreateInfoKHR win32SurfaceCreate{};
    win32SurfaceCreate.sType = vk::StructureType::eWin32SurfaceCreateInfoKHR;

    const GLFWwindow* window = static_cast<const GLFWwindow*>(_window);
    win32SurfaceCreate.hwnd = glfwGetWin32Window(const_cast<GLFWwindow*>(window));
    win32SurfaceCreate.hinstance = GetModuleHandle(nullptr);

    // Create the surface
    vk::Result r = m_Instance.createWin32SurfaceKHR(&win32SurfaceCreate, nullptr, &surface);

    VK_CHECK_CALL(r);
}

Vulkan::VulkanInstance::~VulkanInstance()
{

    m_Instance.destroySurfaceKHR(surface);
#ifdef _DEBUG
    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }
    #endif
    
    m_Instance.destroy();
}



