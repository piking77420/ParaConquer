#include "back_end/vulkan_image.hpp"

void Vulkan::Backend::CreateImage(VulkanContext* _vulkanContext, uint32_t width, uint32_t height, uint32_t mipLevels, vk::ImageType imageType, vk::Format _format ,vk::ImageTiling tiling,
            vk::ImageUsageFlags usage, VmaMemoryUsage vmaMemoryUsage, vk::Image* _image, VmaAllocation* _vmaAllocation)
{

    
    vk::ImageCreateInfo imageInfo{};
    imageInfo.sType = vk::StructureType::eImageCreateInfo;
    imageInfo.imageType = imageType;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = _format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = vmaMemoryUsage;
    allocationCreateInfo.flags = 0;

    VmaAllocationInfo allocationInfo = {};
    
    VK_CALL(static_cast<vk::Result>(vmaCreateImage(_vulkanContext->allocator,
            reinterpret_cast<const VkImageCreateInfo*>(&imageInfo), &allocationCreateInfo,
            reinterpret_cast<VkImage*>(_image), _vmaAllocation, &allocationInfo)));
}
