#include "rendering/vulkan/vulkan_swapchain.hpp"

#include <cassert>
#include <stacktrace>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_wrapper.hpp"

using namespace PC_CORE;

uint32_t VulkanSwapchain::ChooseNumImages(const VkSurfaceCapabilitiesKHR& Capabilities)
{
    Log::Debug("Choose Num Images");

    const uint32_t RequestedNumImages = Capabilities.minImageCount + 1;

    int32_t FinalNumImages = 0;

    if ((Capabilities.maxImageCount > 0) && (RequestedNumImages > Capabilities.maxImageCount))
    {
        FinalNumImages = Capabilities.maxImageCount;
    }
    else
    {
        FinalNumImages = RequestedNumImages;
    }

    return FinalNumImages;
}

VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& PresentModes)
{
    Log::Debug("ChoosePresentMode");

    for (int32_t i = 0; i < PresentModes.size(); i++)
    {
        if (PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return PresentModes[i];
        }
    }

    // Fallback to FIFO which is always supported OpenglStyle 2 buffering
    return VK_PRESENT_MODE_FIFO_KHR;
}

void VulkanSwapchain::CreateFrameBuffer(const VkDevice& _device)
{
    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        const std::array<VkImageView,2> attachments =
        {
            swapChainImages[i].imageView,
            depthImage.textureImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = mainRenderPass.renderPass;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        const VkResult res = vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
        VK_CHECK_ERROR(res,"failed to create framebuffer!");
    }
}


void VulkanSwapchain::Init(const uint32_t widht , const uint32_t _height,const uint32_t _qfamilyIndex,
                               const VkSurfaceKHR& _surface)
{
    PhysicalDevice& _physicalDevice = VulkanInterface::GetPhysicalDevice();
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice.physDevice, VulkanInterface::GetVulkanSurface().surfaceKhr, &_physicalDevice.surfaceCaps);

    
    std::vector<VkSurfaceFormatKHR> formats =
        {
        {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_B8G8R8A8_SRGB,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
        };
    
    const bool resultFormat = VulkanInterface::vulkanPhysicalDevices.IsPresentFormatIsValid(formats,&surfaceFormatKhr);
    if (!resultFormat)
    {
        PC_LOGERROR("None of VkSurfaceFormatKHR asked is been support by the device !!")
    }
    depthFormat = VulkanInterface::vulkanPhysicalDevices.FindDepthFormat();

    
    const VkDevice& device = VulkanInterface::GetDevice().device;
    const VkSurfaceCapabilitiesKHR& SurfaceCaps = _physicalDevice.surfaceCaps;
    nbrOfImage = ChooseNumImages(SurfaceCaps);
    InitRenderPass();
    InitSwapChain(widht, _height, _qfamilyIndex, _surface);
}

void VulkanSwapchain::InitSwapChain(uint32_t widht, uint32_t _height, const uint32_t _qfamilyIndex,
    const VkSurfaceKHR& _surface)
{
     Log::Debug("Creating SwapChain");
     swapChainExtent = {widht, _height};
    
    PhysicalDevice& _physicalDevice = VulkanInterface::GetPhysicalDevice();
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice.physDevice, VulkanInterface::GetVulkanSurface().surfaceKhr, &_physicalDevice.surfaceCaps);
    
    const VkDevice& device = VulkanInterface::GetDevice().device;
    const VkSurfaceCapabilitiesKHR& SurfaceCaps = _physicalDevice.surfaceCaps;
    // Update surfaceCaps 

    const std::vector<VkPresentModeKHR>& PresentModes = _physicalDevice.presentModes;
    const VkPresentModeKHR PresentMode = ChoosePresentMode(PresentModes);
    
    const VkSwapchainCreateInfoKHR SwapChainCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = _surface,
        .minImageCount = nbrOfImage,
        .imageFormat = surfaceFormatKhr.format,
        .imageColorSpace = surfaceFormatKhr.colorSpace,
        .imageExtent = swapChainExtent,
        .imageArrayLayers = 1,
        .imageUsage = (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &_qfamilyIndex,
        .preTransform = SurfaceCaps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = PresentMode,
        .clipped = VK_TRUE
    };

    VkResult res = vkCreateSwapchainKHR(device, &SwapChainCreateInfo, NULL, &swapchainKhr);
    VK_CHECK_ERROR(res, "vkCreateSwapchainKHR");
    Log::Debug("Swap chain created");

    uint32_t NumSwapChainImages = 0;
    res = vkGetSwapchainImagesKHR(device, swapchainKhr, &NumSwapChainImages, NULL);
    VK_CHECK_ERROR(res, "vkGetSwapchainImagesKHR");
    assert(nbrOfImage <= NumSwapChainImages);
    Log::Debug(
        "Requested " + std::to_string(nbrOfImage) + " images, created " + std::to_string(NumSwapChainImages) +
        " images");

    
    swapChainImages.resize(NumSwapChainImages);
    swapChainFramebuffers.resize(NumSwapChainImages);
    std::vector<VkImage> images;
    images.resize(swapChainImages.size());
    res = vkGetSwapchainImagesKHR(device, swapchainKhr, &NumSwapChainImages, images.data());
    VK_CHECK_ERROR(res, "vkGetSwapchainImagesKHR");
    for (size_t i = 0; i < images.size(); i++)
    {
        swapChainImages[i].image = images[i];
        CreateImageView(swapChainImages[i].image, surfaceFormatKhr.format, &swapChainImages[i].imageView, VK_IMAGE_ASPECT_COLOR_BIT);
    }
    InitDepthBuffer();
    
    CreateFrameBuffer(device);
}

void VulkanSwapchain::InitRenderPass()
{
    const Attachment color =
    {
        .attachementIndex = AttachementIndex::Color00,
        .format = surfaceFormatKhr.format,
        .clearOnLoad = true,
        .write = true,
        .imageLayoutInit = VK_IMAGE_LAYOUT_UNDEFINED,
        .imageLayoutRef = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .imageLayoutFinal = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    const Attachment depth =
    {
        .attachementIndex = AttachementIndex::Depth,
        .format = depthFormat,
        .clearOnLoad = true,
        .write = true,
        .imageLayoutInit = VK_IMAGE_LAYOUT_UNDEFINED,
        .imageLayoutRef = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .imageLayoutFinal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    mainRenderPass.Init({ color, depth});

}

void VulkanSwapchain::DestroySwapChain()
{

    const VkDevice& device = VulkanInterface::GetDevice().device;

    depthImage.Destroy();

    Log::Debug("Destroy swapChainImageView SwapChain");
    for (SwapChainImage& i :swapChainImages)
    {
        vkDestroyImageView(device, i.imageView, nullptr);
    }
    
    Log::Debug("Destroy swapChainFrammeBuffer SwapChain");
    for (VkFramebuffer& i : swapChainFramebuffers)
    {
        vkDestroyFramebuffer(device, i, nullptr);
    }
    

    Log::Debug("vkDestroySwapchainKHR");
    vkDestroySwapchainKHR(device, swapchainKhr, nullptr);

}

void VulkanSwapchain::InitDepthBuffer()
{
    const VkImageCreateInfo depthBufferCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = depthFormat,
        .extent = {swapChainExtent.width,swapChainExtent.height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };
    
    depthImage.Init(depthBufferCreateInfo, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void VulkanSwapchain::Destroy()
{
    DestroySwapChain();
    mainRenderPass.Destroy();

}
