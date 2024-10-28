#pragma once

#include "vulkan_header.h"

#include "vulkan_physical_devices.hpp"


namespace Vulkan
{
    // VulkanHarwareWrapper
    class VulkanHarwareWrapper;
    using VHW = VulkanHarwareWrapper;

    class VulkanHarwareWrapper
    {
    public:
        VulkanPhysicalDevices m_VulkanPhysicalDevices;
        
        void Init(const VulkanAppCreateInfo& vulkanMainCreateInfo, VulkanContext* _vulkanContext);
        
        void Destroy(VulkanContext* _context);
        
        static std::vector<vk::DynamicState> GetDynamicState();
    private:

        void CreateInstance(vk::Instance* _outInstance, const char* _AppName, const char* _EngineNamee);

        void CreateDevice(VulkanContext* _vulkanContext);

        void InitVulkanAllocator(VulkanContext* _vulkanContext);

        vk::SurfaceKHR CreateSurface(vk::Instance _currentInstance, void* _windowPtr);

        std::vector<const char*> GetRequiredExtensions() const;

#pragma region DebugCallBack
#ifdef _DEBUG
        bool CheckValidationLayer();

        vk::Result CreateDebugUtilsMessengerEXT(vk::Instance _instance,
                                              const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                              const vk::AllocationCallbacks* pAllocator,
                                              vk::DebugUtilsMessengerEXT* pDebugMessenger);

        void DestroyDebugUtilsMessengerEXT(vk::Instance _instance,
            vk::DebugUtilsMessengerEXT* pDebugMessenger, const vk::AllocationCallbacks* pAllocator);

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void* pUserData);

        void SetUpDebugMessenger(vk::Instance _instance, vk::DebugUtilsMessengerEXT* _debugUtilsMessengerEXT);
        
#endif
#pragma endregion DebugCallBack

    };
}
