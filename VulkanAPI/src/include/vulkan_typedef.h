
#define VK_USE_PLATFORM_WIN32_KHR 
#define NOMINMAX
#include <vulkan/vulkan.hpp>

#define VMA_IMPLEMENTATION
//#define VMA_STATIC_VULKAN_FUNCTIONS 0
//#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
//#include <vma/vk_mem_alloc.h>

namespace VK_NP
{

    constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    constexpr uint32_t INVALID_QUEU = std::numeric_limits<uint32_t>::max();

    struct VulkanAppCreateInfo
    {
        const char* appName;
        const char* engineName;
        void* windowPtr;
    };

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    struct QueuFamiliesIndicies
    {
        uint32_t graphicsFamily = INVALID_QUEU;
        uint32_t presentFamily = INVALID_QUEU;
    };

    struct SwapChainSyncObject
    {
        vk::Semaphore imageAvailableSemaphore;
        vk::Semaphore renderFinishedSemaphore;
        vk::Fence inFlightFence;
    };

    struct VulkanContext
    {
        vk::Instance instance;
        vk::Device device;
        vk::PhysicalDevice physicalDevice;
        //VmaAllocator allocator;
#pragma region DebugCallBack
#ifdef _DEBUG
        vk::DebugUtilsMessengerEXT m_DebugMessenger;
#endif
#pragma endregion DebugCallBack

        QueuFamiliesIndicies queuFamiliesIndicies;
        vk::Queue presentQueue;
        vk::Queue graphicQueue;

        // SwapChain
        vk::SwapchainKHR swapChain;
        vk::SurfaceKHR surface;
        vk::SurfaceFormatKHR m_SurfaceFormat;
        vk::PresentModeKHR m_PresentMode;
        vk::Extent2D m_Extent2D;

        uint32_t swapChainImageCount = 0;
        std::vector<vk::Image> m_SwapchainImages;
        std::vector<vk::ImageView> m_SwapChainImageViews;
        std::vector<vk::Framebuffer> m_SwapChainFramebuffers;
        std::array<SwapChainSyncObject, MAX_FRAMES_IN_FLIGHT> m_syncObject;

        SwapChainSupportDetails swapChainSupportDetails;

        uint32_t imageIndex = 0;
        uint32_t currentFrame = 0;

        vk::RenderPass swapChainRenderPass;

        static VulkanContext* currentContext;

        VulkanContext();

    };



}
