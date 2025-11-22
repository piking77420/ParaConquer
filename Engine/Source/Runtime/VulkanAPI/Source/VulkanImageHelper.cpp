#include "Utils/VulkanImageHelper.hpp"


void Vulkan::Utils::GenerateMipMapFunc(vk::CommandBuffer _commandBuffer, 
                                       vk::Image _image,
                                       vk::ImageLayout newImageLayout,
                                       int32_t _imageWidth,     
                                       int32_t _imageHeight, 
                                       vk::Format _format, 
                                       uint32_t _mipLevel, 
                                       vk::ImageAspectFlags _aspectFlag)
{
    //VkFormatProperties formatProperties;
    //vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

    //if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
    //  throw std::runtime_error("texture image format does not support linear blitting!");
    //}

    vk::ImageMemoryBarrier barrier{};
    barrier.sType = vk::StructureType::eImageMemoryBarrier;
    barrier.image = _image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = _aspectFlag;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = _imageWidth;
    int32_t mipHeight = _imageHeight;

    for (uint32_t i = 1; i < _mipLevel; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

        vk::DependencyFlags depencyFlag{};
        _commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
                                       depencyFlag,
                                       0, nullptr,
                                       0, nullptr,
                                       1, &barrier);

        vk::ImageBlit blit{};
        blit.srcOffsets[0] = vk::Offset3D({0, 0, 0});
        blit.srcOffsets[1] = vk::Offset3D({mipWidth, mipHeight, 1});
        blit.srcSubresource.aspectMask = _aspectFlag;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
        blit.dstOffsets[1] = vk::Offset3D{mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
        blit.dstSubresource.aspectMask = _aspectFlag;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        _commandBuffer.blitImage(
            _image, vk::ImageLayout::eTransferSrcOptimal,
            _image, vk::ImageLayout::eTransferDstOptimal,
            1, &blit,
            vk::Filter::eLinear);

        barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout = newImageLayout;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        vk::DependencyFlags innerLoopDepencyFlag{};
        _commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
            innerLoopDepencyFlag,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = _mipLevel - 1;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = newImageLayout;
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    vk::DependencyFlags depencyFlag{};
    _commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
                                   depencyFlag,
                                   0, nullptr,
                                   0, nullptr,
                                   1, &barrier);
}
