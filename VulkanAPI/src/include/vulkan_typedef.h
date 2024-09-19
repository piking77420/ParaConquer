
#define VK_USE_PLATFORM_WIN32_KHR 
#define NOMINMAX
#include <vulkan/vulkan.hpp>

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

/*

struct VulkanContext
{
    vk::Instance instance;
    vk::Device device;
    vk::PhysicalDevice physicalDevice;
    
    vk::Queue presentQueue;
    vk::Queue graphicQueue;

    // SwapChain
    vk::SwapchainKHR swapChain;
    vk::SurfaceKHR surface;
    vk::SurfaceFormatKHR m_SurfaceFormat;
    vk::PresentModeKHR m_PresentMode;
    vk::Extent2D m_Extent2D;
    
    std::vector<vk::Image> m_SwapchainImages;
    std::vector<vk::ImageView> m_SwapChainImageViews;
    std::vector<vk::Framebuffer> m_SwapChainFramebuffers;

    SwapChainSupportDetails swapChainSupportDetails;
    
    uint32_t imageIndex = 0;
    uint32_t currentFrame = 0;
    //
    

};
*/

