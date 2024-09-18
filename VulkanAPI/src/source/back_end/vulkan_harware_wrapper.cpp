#include "back_end/vulkan_harware_wrapper.hpp"


#include <spirv/spirv_reflect.h>


#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>

VK_NP::VulkanHarwareWrapper* VK_NP::VulkanHarwareWrapper::m_VulkanHarwareWrapperInstance = nullptr;

#ifdef _DEBUG
constexpr std::array<const char*, 1> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};
#endif

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

vk::Queue VK_NP::VulkanHarwareWrapper::GetGraphicQueue()
{
    return m_VulkanHarwareWrapperInstance->graphicQueue;
}

vk::Queue VK_NP::VulkanHarwareWrapper::GetPresentQueu()
{
    return m_VulkanHarwareWrapperInstance->presentQueue;
}

VK_NP::VulkanHarwareWrapper::VulkanHarwareWrapper(const VulkanAppCreateInfo& _vulkanMainCreateInfo)
{
    m_VulkanHarwareWrapperInstance = this;
    
    CreateInstance(_vulkanMainCreateInfo.appName, _vulkanMainCreateInfo.engineName);
    CreateSurface(_vulkanMainCreateInfo.windowPtr);
#ifdef _DEBUG
    SetUpDebugMessenger();
#endif
    m_PhysicalDevices.ChoosePhysicalDevice(m_Instance, m_Surface, deviceExtensions);
    CreateDevice();

}

VK_NP::VulkanHarwareWrapper::~VulkanHarwareWrapper()
{
    m_Device.destroy();
#ifdef _DEBUG
    DestroyDebugUtilsMessengerEXT(m_Instance, &m_DebugMessenger, nullptr);
#endif
    m_Instance.destroySurfaceKHR(m_Surface);
    m_Instance.destroy(nullptr);
}

vk::Instance VK_NP::VulkanHarwareWrapper::GetInstance()
{
    return m_VulkanHarwareWrapperInstance->m_Instance;
}

vk::Device VK_NP::VulkanHarwareWrapper::GetDevice()
{
    return m_VulkanHarwareWrapperInstance->m_Device;
}

const VK_NP::VulkanPhysicalDevices& VK_NP::VulkanHarwareWrapper::GetPhysicalDevices()
{
    return m_VulkanHarwareWrapperInstance->m_PhysicalDevices;   
}


vk::SurfaceKHR VK_NP::VulkanHarwareWrapper::GetSurface()
{
    return m_VulkanHarwareWrapperInstance->m_Surface;
}

SwapChainSupportDetails VK_NP::VulkanHarwareWrapper::GetSwapChainSupportDetailsSurface()
{
    return m_VulkanHarwareWrapperInstance->m_PhysicalDevices.GetSwapChainSupport();
}

void VK_NP::VulkanHarwareWrapper::CreateInstance(const char* _AppName, const char* _EngineName)
{
    vk::ApplicationInfo appInfo = {};

    appInfo.sType = vk::StructureType::eApplicationInfo,
        appInfo.pNext = nullptr,
        appInfo.pApplicationName = _AppName,
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        appInfo.pEngineName = _EngineName,
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0),
        appInfo.apiVersion = VK_API_VERSION_1_3;

    std::vector<const char*> extensions = GetRequiredExtensions();

    vk::InstanceCreateInfo createInfo = {};

    createInfo.sType = vk::StructureType::eInstanceCreateInfo;
    createInfo.pNext = nullptr;
    createInfo.flags = {};
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = 0;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef _DEBUG
    if (ENABLE_VALIDATION_LAYERS && !CheckValidationLayer())
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }
    if (ENABLE_VALIDATION_LAYERS)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }
#endif

    vk::Result result = vk::createInstance(&createInfo, nullptr, &m_Instance);
    VK_CALL(result);



}

void VK_NP::VulkanHarwareWrapper::CreateDevice()
{
    vk::PhysicalDevice physicalDevice = m_PhysicalDevices.GetSelectedPhysicalDevice();
    const QueuFamiliesIndicies queuFamiliesIndicies = m_PhysicalDevices.GetQueueFamiliesIndicies();
    vk::PhysicalDeviceFeatures physicalDeviceFeatures = physicalDevice.getFeatures();
    
    std::vector<uint32_t> uniqueQueueFamilyIndices = { queuFamiliesIndicies.graphicsFamily , queuFamiliesIndicies.presentFamily };
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    
    
    for (uint32_t uniqueQueueFamilyIndex : uniqueQueueFamilyIndices)
    {
        vk::DeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;
    
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures = {};
    
    vk::DeviceCreateInfo vkDevicecreateInfo = {};
    vkDevicecreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    vkDevicecreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    vkDevicecreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    vkDevicecreateInfo.enabledExtensionCount = 0;
    vkDevicecreateInfo.pEnabledFeatures = &deviceFeatures;
    vkDevicecreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    vkDevicecreateInfo.ppEnabledExtensionNames = deviceExtensions.data();


#pragma region DeviceLayer
#ifdef _DEBUG
    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        vkDevicecreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        vkDevicecreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        vkDevicecreateInfo.enabledLayerCount = 0;

    }
    #endif
#pragma endregion DeviceLayer
    
    VK_CALL(physicalDevice.createDevice(&vkDevicecreateInfo, nullptr, &m_Device));
    m_Device.getQueue(queuFamiliesIndicies.graphicsFamily, 0, &graphicQueue);
    m_Device.getQueue(queuFamiliesIndicies.presentFamily, 0, &presentQueue);
}


void VK_NP::VulkanHarwareWrapper::InitVulkanAllocator()
{

}

void VK_NP::VulkanHarwareWrapper::CreateSurface(void* _windowPtr)
{
    VkWin32SurfaceCreateInfoKHR   createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = glfwGetWin32Window(static_cast<GLFWwindow*>(_windowPtr));
    createInfo.hinstance = GetModuleHandle(nullptr);

    m_Surface = VulkanHarwareWrapper::GetInstance().createWin32SurfaceKHR(createInfo, nullptr);
}

std::vector<const char*> VK_NP::VulkanHarwareWrapper::GetRequiredExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;  
}

#pragma region DebugCallBack

#ifdef _DEBUG

bool VK_NP::VulkanHarwareWrapper::CheckValidationLayer()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

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

vk::Result VK_NP::VulkanHarwareWrapper::CreateDebugUtilsMessengerEXT(const vk::Instance _instance,
                                                                     const vk::DebugUtilsMessengerCreateInfoEXT*
                                                                     pCreateInfo,
                                                                     const vk::AllocationCallbacks* pAllocator,
                                                                     vk::DebugUtilsMessengerEXT* pDebugMessenger)
{
    // Ensure the instance supports VK_EXT_debug_utils extension
    if (!_instance)
    {
        return vk::Result::eErrorInitializationFailed;
    }

    vk::DispatchLoaderDynamic dldi(_instance, vkGetInstanceProcAddr);

    return _instance.createDebugUtilsMessengerEXT(pCreateInfo, pAllocator, pDebugMessenger, dldi);
}

void VK_NP::VulkanHarwareWrapper::DestroyDebugUtilsMessengerEXT(const vk::Instance _instance,
                                                                vk::DebugUtilsMessengerEXT* pDebugMessenger,
                                                                const vk::AllocationCallbacks* pAllocator)
{
    // Create the DispatchLoaderDynamic to load the extension functions
    vk::DispatchLoaderDynamic dldi(_instance, vkGetInstanceProcAddr);

    // Call destroyDebugUtilsMessengerEXT using the dynamic loader
    if (pDebugMessenger != nullptr)
    {
        _instance.destroyDebugUtilsMessengerEXT(*pDebugMessenger, pAllocator, dldi);
    }
}

VkBool32 VK_NP::VulkanHarwareWrapper::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void VK_NP::VulkanHarwareWrapper::SetUpDebugMessenger()
{
    if constexpr (!ENABLE_VALIDATION_LAYERS)
        return;

    vk::DebugUtilsMessengerCreateInfoEXT createInfo = {};

    createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
    
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
    | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation ;
        
    createInfo.pfnUserCallback = DebugCallback;
    createInfo.pUserData = nullptr; // O

    VK_CALL(CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger));
}


#endif
#pragma endregion DebugCallBack
