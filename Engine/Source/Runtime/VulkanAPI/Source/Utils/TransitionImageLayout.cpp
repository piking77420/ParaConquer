#include "Utils/TransitionImageLayout.hpp"

#include "Utils/RhiToVulkan.hpp"
#include "VulkanTexture.hpp"



static void PopulateBarrier(
    vk::ImageMemoryBarrier* _Barrier, 
    vk::PipelineStageFlags* _SourceStage, 
    vk::PipelineStageFlags* _DestinationStage)
{
    assert(_Barrier != nullptr && _SourceStage != nullptr && _DestinationStage != nullptr);

    if (_Barrier->oldLayout == vk::ImageLayout::eUndefined && _Barrier->newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        _Barrier->srcAccessMask = {};
        _Barrier->dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        *_SourceStage= vk::PipelineStageFlagBits::eTopOfPipe;
        *_DestinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (_Barrier->oldLayout == vk::ImageLayout::eTransferDstOptimal && _Barrier->newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        _Barrier->srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        _Barrier->dstAccessMask = vk::AccessFlagBits::eShaderRead;

        *_SourceStage = vk::PipelineStageFlagBits::eTransfer;
        *_DestinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (_Barrier->oldLayout == vk::ImageLayout::eUndefined && _Barrier->newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        _Barrier->srcAccessMask = {};
        _Barrier->dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead |
            vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        *_SourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        *_DestinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else if (_Barrier->oldLayout == vk::ImageLayout::eUndefined && _Barrier->newLayout == vk::ImageLayout::eColorAttachmentOptimal)
    {
        _Barrier->srcAccessMask = {};
        _Barrier->dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

        *_SourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        *_DestinationStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    }
    else if (_Barrier->oldLayout == vk::ImageLayout::eUndefined && _Barrier->newLayout == vk::ImageLayout::eDepthAttachmentOptimal)
    {
        _Barrier->srcAccessMask = {};
        _Barrier->dstAccessMask =
            vk::AccessFlagBits::eDepthStencilAttachmentRead |
            vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        *_SourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        *_DestinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else if (_Barrier->oldLayout == vk::ImageLayout::eUndefined && _Barrier->newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        _Barrier->srcAccessMask = {};
        _Barrier->dstAccessMask = vk::AccessFlagBits::eShaderRead;

        *_SourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        *_DestinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

}



void Vulkan::TransitionImageLayout(vk::CommandBuffer _commandBuffer, vk::Image image, vk::Format format,
                                   vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                                   vk::ImageAspectFlags imageAspect, uint32_t _layerCount, uint32_t _levelCount)
{
    if (oldLayout == newLayout)
        return;

    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    vk::ImageMemoryBarrier barrier{};
    barrier.sType = vk::StructureType::eImageMemoryBarrier;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = imageAspect;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = _levelCount;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = _layerCount;

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    PopulateBarrier(&barrier, &sourceStage, &destinationStage);

   
    _commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, 0, nullptr,
                                   0, nullptr,
                                   1, &barrier);
}

bool Vulkan::HasStencilComponent(vk::Format _format)
{
    return _format == vk::Format::eD32Sfloat || _format == vk::Format::eD24UnormS8Uint;
}
/*/
void Vulkan::TransitionImageLayout(vk::CommandBuffer commandBuffer, const PC_CORE::ImageStateTransition& _ImageStateTransition, size_t _FrameIndex)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    assert(_ImageStateTransition.Texture != nullptr);

    VulkanTexture* vulkanTexture = reinterpret_cast<VulkanTexture*>(_ImageStateTransition.Texture);

    vk::Image image = static_cast<Vulkan::TextureAndAlloc*>(vulkanTexture->GetFrameNativeHandle(_FrameIndex))->Image;

    vk::ImageMemoryBarrier barrier{};
    barrier.sType = vk::StructureType::eImageMemoryBarrier;
    barrier.oldLayout = Utils::RhiResourceStateToVulkanImageLayout(_ImageStateTransition.OldState);
    barrier.newLayout = Utils::RhiResourceStateToVulkanImageLayout(_ImageStateTransition.NewState);
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = vulkanTexture->VkImageAspectFlags;
    barrier.subresourceRange.baseMipLevel = _ImageStateTransition.FirstMipLevel;
    barrier.subresourceRange.levelCount = _ImageStateTransition.MipLevelsCount;
    barrier.subresourceRange.baseArrayLayer = _ImageStateTransition.FirstArraySlice;
    barrier.subresourceRange.layerCount = _ImageStateTransition.ArraySliceCount;


    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    PopulateBarrier(&barrier, &sourceStage, &destinationStage);

    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, 0, nullptr,
        0, nullptr,
        1, &barrier);
}
*/

