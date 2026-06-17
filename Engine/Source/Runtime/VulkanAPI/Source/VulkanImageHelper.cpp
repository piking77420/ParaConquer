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
    if (_MipLevel <= 1)
        return;

    vk::ImageMemoryBarrier barrier{};
    barrier.sType = vk::StructureType::eImageMemoryBarrier;
    barrier.image = _Image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = _AspectFlag;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.layerCount = 1;

    for (uint32_t layer = 0; layer < _LayerCount; ++layer)
    {
        int32_t mipWidth = _ImageWidth;
        int32_t mipHeight = _ImageHeight;

        for (uint32_t mip = 1; mip < _MipLevel; ++mip)
        {
            
            //Destination mip must be TRANSFER_DST_OPTIMAL.
            barrier.subresourceRange.baseArrayLayer = layer;
            barrier.subresourceRange.baseMipLevel = mip;
            barrier.oldLayout = (_OldImageLayout != vk::ImageLayout::eUndefined && layer == 0 && mip == 1) ? _OldImageLayout : vk::ImageLayout::eUndefined;
            barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            _CommandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTopOfPipe,
                vk::PipelineStageFlagBits::eTransfer,
                {},
                nullptr,
                nullptr,
                barrier);

            
            // Previous mip becomes the blit source.
            // mip 0 comes from the caller's old layout
            
            barrier.subresourceRange.baseMipLevel = mip - 1;
            barrier.oldLayout = (mip == 1)
                ? _OldImageLayout
                : vk::ImageLayout::eTransferDstOptimal;
            barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.srcAccessMask = (mip == 1)
                ? _OldAccesFlag
                : vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            _CommandBuffer.pipelineBarrier(
                (mip == 1) ? _OldPipelineStageFlags : vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eTransfer,
                {},
                nullptr,
                nullptr,
                barrier);

            vk::ImageBlit blit{};

            blit.srcOffsets[0] = vk::Offset3D{ 0, 0, 0 };
            blit.srcOffsets[1] = vk::Offset3D{ mipWidth, mipHeight, 1 };

            blit.srcSubresource.aspectMask = _AspectFlag;
            blit.srcSubresource.mipLevel = mip - 1;
            blit.srcSubresource.baseArrayLayer = layer;
            blit.srcSubresource.layerCount = 1;

            blit.dstOffsets[0] = vk::Offset3D{ 0, 0, 0 };
            blit.dstOffsets[1] = vk::Offset3D{
                mipWidth > 1 ? mipWidth / 2 : 1,
                mipHeight > 1 ? mipHeight / 2 : 1,
                1
            };

            blit.dstSubresource.aspectMask = _AspectFlag;
            blit.dstSubresource.mipLevel = mip;
            blit.dstSubresource.baseArrayLayer = layer;
            blit.dstSubresource.layerCount = 1;

            _CommandBuffer.blitImage(
                _Image,
                vk::ImageLayout::eTransferSrcOptimal,
                _Image,
                vk::ImageLayout::eTransferDstOptimal,
                blit,
                _Filter);

            
            //Previous mip is done. Move it to final layout.
            barrier.subresourceRange.baseMipLevel = mip - 1;
            barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.newLayout = _NewImageLayout;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = _NewAccesFlag;

            _CommandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer,
                _NewPipelineStageFlags,
                {},
                nullptr,
                nullptr,
                barrier);

            if (mipWidth > 1)  mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        
        // Last mip was only written as TRANSFER_DST_OPTIMAL.
        //Move it to final layout.
        barrier.subresourceRange.baseArrayLayer = layer;
        barrier.subresourceRange.baseMipLevel = _MipLevel - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = _NewImageLayout;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = _NewAccesFlag;

        _CommandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            _NewPipelineStageFlags,
            {},
            nullptr,
            nullptr,
            barrier);
    }
}
