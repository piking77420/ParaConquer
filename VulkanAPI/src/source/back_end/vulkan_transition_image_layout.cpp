#include "back_end/vulkan_transition_image_layout.hpp"

#include "back_end/vulkan_command_pool_function.hpp"

void Vulkan::Backend::TransitionImageLayout(VulkanContext* _context, vk::Image _imageHandle,
                                            vk::ImageAspectFlagBits _imageAspectFlagBits, uint32_t _mipLevel,
                                            vk::ImageLayout _initialLayout, vk::ImageLayout _finalLayout)
{
    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(_context->device,
                                                              _context->resourceCommandPool);

    vk::ImageMemoryBarrier barrier{};
    barrier.sType = vk::StructureType::eImageMemoryBarrier;
    barrier.oldLayout = _initialLayout;
    barrier.newLayout = _finalLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = _imageHandle;
    barrier.subresourceRange.aspectMask = _imageAspectFlagBits;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = _mipLevel;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    // TO DO MAKE IT CLEANER
    if (_initialLayout == vk::ImageLayout::eUndefined && _finalLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eNone;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (_initialLayout == vk::ImageLayout::eTransferDstOptimal && _finalLayout ==
        vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (_initialLayout == vk::ImageLayout::eUndefined && _finalLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead & vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, 0, nullptr, 0, nullptr, 1,
                                  &barrier);

    EndSingleTimeCommands(commandBuffer, _context->device, _context->resourceFence,
                          _context->vkQueues.graphicQueue);
}
