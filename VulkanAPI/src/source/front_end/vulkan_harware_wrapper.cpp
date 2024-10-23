#include "front_end/vulkan_harware_wrapper.hpp"


#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>


#ifdef _DEBUG
constexpr std::array<const char*, 1> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};
#endif

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};



void VK_NP::VulkanHarwareWrapper::Init(const VulkanAppCreateInfo& vulkanMainCreateInfo, VulkanContext* _vulkanContext)
{
    CreateInstance(&_vulkanContext->instance, vulkanMainCreateInfo.appName, vulkanMainCreateInfo.engineName);
    _vulkanContext->surface = CreateSurface(_vulkanContext->instance, vulkanMainCreateInfo.windowPtr);
#ifdef _DEBUG
    SetUpDebugMessenger(_vulkanContext->instance, &_vulkanContext->m_DebugMessenger);
#endif
    _vulkanContext->physicalDevice = m_VulkanPhysicalDevices.ChoosePhysicalDevice(_vulkanContext,  deviceExtensions);
    _vulkanContext->queuFamiliesIndicies = m_VulkanPhysicalDevices.FindQueuFamillies(
        _vulkanContext->physicalDevice, _vulkanContext->surface);
    CreateDevice(_vulkanContext);
    InitVulkanAllocator(_vulkanContext);
}

void VK_NP::VulkanHarwareWrapper::Destroy(VulkanContext* _context)
{
    vmaDestroyAllocator(_context->allocator);
    _context->device.destroy();
#ifdef _DEBUG
    DestroyDebugUtilsMessengerEXT(_context->instance, &_context->m_DebugMessenger, nullptr);
#endif
    _context->instance.destroySurfaceKHR(_context->surface);
    _context->instance.destroy(nullptr);
}

std::vector<vk::DynamicState> VK_NP::VulkanHarwareWrapper::GetDynamicState()
{
    return
    {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };
}


void VK_NP::VulkanHarwareWrapper::CreateInstance(vk::Instance* _outInstance, const char* _AppName,
                                                 const char* _EngineName)
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

    VK_CALL(vk::createInstance(&createInfo, nullptr, _outInstance));
}

void VK_NP::VulkanHarwareWrapper::CreateDevice(VulkanContext* _vulkanContext)
{
    vk::PhysicalDevice physicalDevice = _vulkanContext->physicalDevice;
    vk::PhysicalDeviceFeatures physicalDeviceFeatures = physicalDevice.getFeatures();

    std::vector<uint32_t> uniqueQueueFamilyIndices = {
        _vulkanContext->queuFamiliesIndicies.graphicsFamily,
        _vulkanContext->queuFamiliesIndicies.presentFamily,
        _vulkanContext->queuFamiliesIndicies.transferFamily
    };

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

    VK_CALL(physicalDevice.createDevice(&vkDevicecreateInfo, nullptr, &_vulkanContext->device));
    
    _vulkanContext->vkQueues.graphicQueue = _vulkanContext->device.getQueue(_vulkanContext->queuFamiliesIndicies.graphicsFamily, 0);
    _vulkanContext->vkQueues.presentQueue = _vulkanContext->device.getQueue(_vulkanContext->queuFamiliesIndicies.presentFamily, 0);
    _vulkanContext->vkQueues.transferQueu = _vulkanContext->device.getQueue(_vulkanContext->queuFamiliesIndicies.transferFamily, 0);

}


void VK_NP::VulkanHarwareWrapper::InitVulkanAllocator(VulkanContext* _vulkanContext)
{
    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    allocatorCreateInfo.physicalDevice = _vulkanContext->physicalDevice;
    allocatorCreateInfo.device = _vulkanContext->device;
    allocatorCreateInfo.instance = _vulkanContext->instance;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

    VmaAllocator allocator;
    VK_CALL(static_cast<vk::Result>(vmaCreateAllocator(&allocatorCreateInfo, &allocator)));

    _vulkanContext->allocator = allocator;
}

vk::SurfaceKHR VK_NP::VulkanHarwareWrapper::CreateSurface(vk::Instance _currentInstance, void* _windowPtr)
{
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = glfwGetWin32Window(static_cast<GLFWwindow*>(_windowPtr));
    createInfo.hinstance = GetModuleHandle(nullptr);

    return _currentInstance.createWin32SurfaceKHR(createInfo, nullptr);
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

void VK_NP::VulkanHarwareWrapper::SetUpDebugMessenger(vk::Instance _instance,
                                                      vk::DebugUtilsMessengerEXT* _debugUtilsMessengerEXT)
{
    if constexpr (!ENABLE_VALIDATION_LAYERS)
        return;

    vk::DebugUtilsMessengerCreateInfoEXT createInfo = {};

    createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;

    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

    createInfo.pfnUserCallback = DebugCallback;
    createInfo.pUserData = nullptr; // O

    VK_CALL(CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, _debugUtilsMessengerEXT));
}


#endif
#pragma endregion DebugCallBack
