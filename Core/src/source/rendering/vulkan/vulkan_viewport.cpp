#include "rendering/vulkan/vulkan_viewport.hpp"

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE; 

VulkanViewport::VulkanViewport()
{
    CreateViewPortImageAndFrameBuffer();
}

VulkanViewport::~VulkanViewport()
{
    DestroyViewPortImageAndFrameBuffer();
}

void VulkanViewport::HandleResize(Vector2i _windowSize)
{
    if (_windowSize == viewPortSize)
        return;

    Log::Debug("ResizeViewPort");
    viewPortSize = _windowSize;
    DestroyViewPortImageAndFrameBuffer();
    CreateViewPortImageAndFrameBuffer();
}

void VulkanViewport::CreateViewPortImageAndFrameBuffer()
{
    const uint32_t nbrOfImage = VulkanInterface::GetNbrOfImage();
    viewPortTexture.resize(nbrOfImage);
    framebuffers.resize(viewPortTexture.size());
    
    VkImageCreateInfo  vkImageCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VulkanInterface::vulkanSwapChapchain.surfaceFormatKhr.format,
        .extent = {static_cast<uint32_t>(viewPortSize.x), static_cast<uint32_t>(viewPortSize.y),static_cast<uint32_t>(1)},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_LINEAR,
        .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    };

    for (VulkanTexture& v : viewPortTexture)
    {
        v.Init(vkImageCreateInfo);
    }

    for (uint32_t i = 0; i < framebuffers.size(); ++i)
    {
        const std::array<VkImageView,1> attachments =
        {
            viewPortTexture[i].textureImageView,
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = VulkanInterface::vulkanSwapChapchain.mainRenderPass.renderPass;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = viewPortSize.x;
        framebufferInfo.height = viewPortSize.y;
        framebufferInfo.layers = 1;

        const VkResult res = vkCreateFramebuffer(VulkanInterface::GetDevice().device, &framebufferInfo, nullptr, &framebuffers[i]);
        VK_CHECK_ERROR(res,"failed to create framebuffer!");
    }
}

void VulkanViewport::DestroyViewPortImageAndFrameBuffer()
{
    for (VulkanTexture& v : viewPortTexture)
    {
        v.Destroy();
    }

    for (VkFramebuffer& i : framebuffers)
    {
        vkDestroyFramebuffer(VulkanInterface::GetDevice().device, i, nullptr);
    }
}
