#include "Utils/VulkanImageHelper.hpp"


void Vulkan::Utils::GenerateMipMapFunc(vk::CommandBuffer _CommandBuffer,
    vk::Image _Image,
    vk::Filter _Filter,
    vk::AccessFlags _OldAccesFlag,
    vk::ImageLayout _OldImageLayout,
    vk::PipelineStageFlags _OldPipelineStageFlags,
    vk::AccessFlags _NewAccesFlag,
    vk::ImageLayout _NewImageLayout,
    vk::PipelineStageFlags _NewPipelineStageFlags,
    int32_t _ImageWidth,
    int32_t _ImageHeight,
    vk::Format _Format,
    uint32_t _MipLevel,
    uint32_t _LayerCount,
    vk::ImageAspectFlags _AspectFlag
)
{

    for (size_t layer = 0; layer < _LayerCount; layer++)
    {
        vk::ImageMemoryBarrier barrier{};
    barrier.sType = vk::StructureType::eImageMemoryBarrier;
    barrier.image = _Image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = _AspectFlag;
    barrier.subresourceRange.baseArrayLayer = layer;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = _ImageWidth;
    int32_t mipHeight = _ImageHeight;

    for (uint32_t mip = 1; mip < _MipLevel; mip++)
    {
        barrier.subresourceRange.baseMipLevel = mip - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

        vk::DependencyFlags depencyFlag{};
        _CommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
                                       depencyFlag,
                                       0, nullptr,
                                       0, nullptr,
                                       1, &barrier);

        vk::ImageBlit blit{};
        blit.srcOffsets[0] = vk::Offset3D({0, 0, 0});
        blit.srcOffsets[1] = vk::Offset3D({mipWidth, mipHeight, 1});
        blit.srcSubresource.aspectMask = _AspectFlag;
        blit.srcSubresource.mipLevel = mip - 1;
        blit.srcSubresource.baseArrayLayer = layer;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
        blit.dstOffsets[1] = vk::Offset3D{mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
        blit.dstSubresource.aspectMask = _AspectFlag;
        blit.dstSubresource.mipLevel = mip;
        blit.dstSubresource.baseArrayLayer = layer;
        blit.dstSubresource.layerCount = 1;

        _CommandBuffer.blitImage(
            _Image, vk::ImageLayout::eTransferSrcOptimal,
            _Image, vk::ImageLayout::eTransferDstOptimal,
            1, &blit,
            _Filter);

        barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout = _NewImageLayout;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = _NewAccesFlag;

        vk::DependencyFlags innerLoopDepencyFlag{};
        _CommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, _NewPipelineStageFlags,
            innerLoopDepencyFlag,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    // last mip
    barrier.subresourceRange.baseMipLevel = _MipLevel - 1;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = _NewImageLayout;
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = _NewAccesFlag;

    vk::DependencyFlags depencyFlag{};
    _CommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, _NewPipelineStageFlags,
                                   depencyFlag,
                                   0, nullptr,
                                   0, nullptr,
                                   1, &barrier);
    }
    
}
