#include "texture/vulkan_texture.hpp"

#include "vulkan_context.hpp"

Vulkan::VulkanTexture::VulkanTexture(PC_CORE::MemoryLocalisation _usage)
{
    //TODO
    textureAndAlloc.resize(MAX_FRAMES_IN_FLIGHT);
}

Vulkan::VulkanTexture::~VulkanTexture()
{
    for (auto& textureAndAlloc : textureAndAlloc)
    {
        if (textureAndAlloc.allocation == VK_NULL_HANDLE || textureAndAlloc.image == VK_NULL_HANDLE)
            return;

        auto& context = VulkanContext::GetContext();
        vk::Device device = context.GetDevice()->GetDevice();

        device.destroyImageView(textureAndAlloc.imageView);
        vmaDestroyImage(context.allocator, textureAndAlloc.image, textureAndAlloc.allocation);

        textureAndAlloc.allocation = VK_NULL_HANDLE;
        textureAndAlloc.imageView = VK_NULL_HANDLE;
        textureAndAlloc.image = VK_NULL_HANDLE;
    }
}
