#pragma once

#include "vulkan_header.h"
#include <vma/vk_mem_alloc.h>

#include "vulkan_physical_devices.hpp"


namespace VK_NP
{
    // VulkanHarwareWrapper
    class VulkanHarwareWrapper;
    using VHW = VulkanHarwareWrapper;

    class VulkanHarwareWrapper
    {
    public:

        static vk::Queue GetGraphicQueue();

        static vk::Queue GetPresentQueu();
        
        VulkanHarwareWrapper(const VulkanAppCreateInfo& vulkanMainCreateInfo);

        ~VulkanHarwareWrapper();

        static vk::Instance GetInstance();

        static vk::Device GetDevice();

        static const VulkanPhysicalDevices& GetPhysicalDevices();

        static vk::SurfaceKHR GetSurface();

        static SwapChainSupportDetails GetSwapChainSupportDetailsSurface();

    private:
        void CreateInstance(const char* _AppName, const char* _EngineNamee);

        void CreateDevice();

        void InitVulkanAllocator();

        void CreateSurface(void* _windowPtr);

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

        void SetUpDebugMessenger();
        
        vk::DebugUtilsMessengerEXT m_DebugMessenger;
#endif
#pragma endregion DebugCallBack

        vk::Instance m_Instance;

        VulkanPhysicalDevices m_PhysicalDevices;

        vk::Device m_Device;

        VmaAllocator vmaAllocator;
        
        vk::SurfaceKHR m_Surface;

        vk::Queue graphicQueue;

        vk::Queue presentQueue;

        static VulkanHarwareWrapper* m_VulkanHarwareWrapperInstance;
    };
}
