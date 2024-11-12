#include "back_end/vulkan_image.hpp"

#include "back_end/vulkan_command_pool_function.hpp"

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

void Vulkan::Backend::GenerateMipmaps(vk::Image _image, vk::Format _imageFormat, int32_t _texWidth, int32_t _texHeight, uint32_t _mipLevels,
   vk::PhysicalDevice _physicalDevice, vk::Device _device ,vk::CommandPool _commandPool, vk::Fence _fence, vk::Queue _queue)
{
    vk::FormatProperties formatProperties;
   _physicalDevice.getFormatProperties(_imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(_device, _commandPool);

    vk::ImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = vk::StructureType::eImageMemoryBarrier;
    imageMemoryBarrier.image = _image;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = _texWidth;
    int32_t mipHeight = _texHeight;

    for (uint32_t i = 1; i < _mipLevels; i++)
    {
            imageMemoryBarrier.subresourceRange.baseMipLevel = i - 1;
            imageMemoryBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
            imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {},
                0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

            vk::ImageBlit blit{};
            blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
            blit.srcOffsets[1] = vk::Offset3D{mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
            blit.dstOffsets[1] = vk::Offset3D{ mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            commandBuffer.blitImage(_image, vk::ImageLayout::eTransferSrcOptimal, _image, vk::ImageLayout::eTransferDstOptimal,
            1, &blit, vk::Filter::eLinear);


            imageMemoryBarrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
            imageMemoryBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,vk::PipelineStageFlagBits::eFragmentShader, {},
        0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

    imageMemoryBarrier.subresourceRange.baseMipLevel = _mipLevels - 1;
    imageMemoryBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    imageMemoryBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {},
        0, nullptr,
        0, nullptr,
        1, &imageMemoryBarrier);

    
    EndSingleTimeCommands(commandBuffer, _device, _fence, _queue);

    
}
