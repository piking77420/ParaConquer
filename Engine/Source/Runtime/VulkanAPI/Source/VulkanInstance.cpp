#include "VulkanInstance.hpp"

#include "Io/Window.hpp"
#include <memory>

#include "PerfRegion.hpp"

#if defined(_WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__)
    #define VK_USE_PLATFORM_XLIB_KHR
#endif
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>


#if defined(_WIN32)
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__linux__)
    #define GLFW_EXPOSE_NATIVE_X11
#endif

#ifdef DEBUG_GPU_ON


static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
    {
        PC_LOG("[validation layer {} ] : {}", "Verbose", pCallbackData->pMessage)
    }
    else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
    {
        PC_LOG("[validation layer {} ] : {}", "Info", pCallbackData->pMessage)
    }
    else
    {
        PC_LOGERROR("[validation layer] : {}", pCallbackData->pMessage);
    }

    return VK_FALSE;
}


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void Vulkan::VulkanInstance::SetupDebugMessenger()
{
    if constexpr (!ENABLE_VALIDATION_LAYERS)
        return;

    vk::DebugUtilsMessengerCreateInfoEXT createInfo = {};
    PopulateDebugMessengerCreateInfo(createInfo);
    VK_CHECK_CALL(
        static_cast<vk::Result>(CreateDebugUtilsMessengerEXT(m_Instance, reinterpret_cast<
            VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, reinterpret_cast<VkDebugUtilsMessengerEXT*>(&
                m_DebugMessenger))));
}

void Vulkan::VulkanInstance::PopulateDebugMessengerCreateInfo(
    vk::DebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;


    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
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

void Vulkan::VulkanInstance::GetDebugFunc()
{
#ifdef  DEBUG_GPU_ON
    m_BeginDebugLabel = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(
        m_Instance, "vkCmdBeginDebugUtilsLabelEXT"));
    m_EndDebugLabel = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(
        m_Instance, "vkCmdEndDebugUtilsLabelEXT"));
    m_DebugName = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(
        m_Instance, "vkSetDebugUtilsObjectNameEXT"));


    if (m_EndDebugLabel == nullptr || m_BeginDebugLabel == nullptr || m_DebugName == nullptr)
    {
        PC_LOGERROR("Enable to get debgu label func ");
        assert(false);
    }
#endif

#ifdef  PROFILING

    m_Gpdctd = reinterpret_cast<PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>(vkGetInstanceProcAddr(
        m_Instance, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"));
    if (m_Gpdctd == nullptr)
    {
        PC_LOGERROR("Enable to get Profiling func ");
        assert(false);
    }
#endif
}

Vulkan::VulkanInstance::VulkanInstance(const PC_CORE::RenderInstanceCreateInfo& _renderInstanceCreateInfo,
                                       GLFWwindow* _window) : RenderInstance(_renderInstanceCreateInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

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
#ifdef DEBUG_GPU_ON

    VkValidationFeatureEnableEXT enables[] = {
    VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
    VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT
    };
    VkValidationFeaturesEXT validationFeatures{};
    validationFeatures.sType =
        VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
    validationFeatures.enabledValidationFeatureCount =
        static_cast<uint32_t>(std::size(enables));
    validationFeatures.pEnabledValidationFeatures = enables;

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
    instanceCreateInfo.ppEnabledLayerNames = nullptr;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

#ifdef DEBUG_GPU_ON
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

  // instanceCreateInfo.pNext = &validationFeatures;

#endif
    VK_CHECK_CALL(vk::createInstance(&instanceCreateInfo, nullptr, &m_Instance));

#ifdef DEBUG_GPU_ON
    SetupDebugMessenger();
    GetDebugFunc();
#endif

    m_DrawMeshTasksTExt = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(vkGetInstanceProcAddr(
        m_Instance, "vkCmdDrawMeshTasksEXT"));

    InitSurface(_window);
}


#ifdef  DEBUG_GPU_ON

void Vulkan::VulkanInstance::SetDebugName(vk::Device _device, const vk::DebugUtilsObjectNameInfoEXT* _pNameInfo) const
{
    if (m_DebugName != nullptr)
        m_DebugName(_device, reinterpret_cast<const VkDebugUtilsObjectNameInfoEXT*>(_pNameInfo));
}
#endif

void Vulkan::VulkanInstance::InitSurface(GLFWwindow* _window)
{
    if (_window == nullptr)
    {
        throw std::invalid_argument("Window handle is null");
    }

    vk::Win32SurfaceCreateInfoKHR win32SurfaceCreate{};
    win32SurfaceCreate.sType = vk::StructureType::eWin32SurfaceCreateInfoKHR;

    win32SurfaceCreate.hwnd = glfwGetWin32Window(const_cast<GLFWwindow*>(_window));
    win32SurfaceCreate.hinstance = GetModuleHandle(nullptr);

    // Create the surface
    vk::Result r = m_Instance.createWin32SurfaceKHR(&win32SurfaceCreate, nullptr, &surface);

    VK_CHECK_CALL(r);
}

Vulkan::VulkanInstance::~VulkanInstance()
{
    m_Instance.destroySurfaceKHR(surface);
#ifdef DEBUG_GPU_ON
    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }
#endif

    m_Instance.destroy();
}
