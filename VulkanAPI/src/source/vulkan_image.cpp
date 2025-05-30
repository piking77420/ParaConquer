#include "vulkan_image.hpp"

void Vulkan::CreateImage(VmaAllocator allocatore, uint32_t width, uint32_t height, uint32_t depth, uint32_t _mimpLevel,
    vk::SampleCountFlagBits _sampleCount, vk::ImageType _imageType, vk::Format format, vk::ImageTiling tiling,
    vk::ImageUsageFlags usage, VmaMemoryUsage imageMemory, VkImage* _outImage, VmaAllocation* _outAllocation)
{
    vk::ImageCreateInfo imageInfo{};
    imageInfo.sType = vk::StructureType::eImageCreateInfo;
    imageInfo.imageType = _imageType;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = depth;
    imageInfo.mipLevels = _mimpLevel;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.samples = _sampleCount;
    
    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = imageMemory;
    
   
    vmaCreateImage(allocatore, reinterpret_cast<VkImageCreateInfo*>(&imageInfo), &allocationInfo, _outImage, _outAllocation, nullptr);
}

vk::ImageView Vulkan::CreateImageView(vk::Device _device, vk::Image _image, vk::ImageViewType _imageType,
                                      vk::Format _format, vk::ImageAspectFlags imageAspect, uint32_t _mipLevels)
{
    vk::ImageViewCreateInfo imageInfo{};
    imageInfo.sType  = vk::StructureType::eImageViewCreateInfo;
    imageInfo.image = _image;
    imageInfo.viewType = _imageType;
    imageInfo.format = _format;
    imageInfo.subresourceRange.aspectMask = imageAspect;
    imageInfo.subresourceRange.baseMipLevel = 0;
    imageInfo.subresourceRange.baseArrayLayer = 0;
    imageInfo.subresourceRange.layerCount = 1;
    imageInfo.subresourceRange.levelCount = _mipLevels;

    vk::ImageView imageView;

    VK_CALL(_device.createImageView(&imageInfo, nullptr, &imageView));

    return imageView;
}

void Vulkan::GetTextureUsage(const PC_CORE::CreateImageInfo& _createTextureInfo, VmaMemoryUsage* _memoryUsage,
    vk::ImageUsageFlags* _usage, vk::ImageLayout* _finalLoayout, vk::ImageAspectFlags* _imageAspectFlag)
{
    *_memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;

    switch (_createTextureInfo.textureNature)
    {
    case PC_CORE::TextureNature::Default:
        *_usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
        *_finalLoayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        *_imageAspectFlag = vk::ImageAspectFlagBits::eColor;
        break;
    case PC_CORE::TextureNature::RenderTarget:
        switch (_createTextureInfo.textureAttachement)
        {
    case PC_CORE::TextureAttachement::None:
            break;
    case PC_CORE::TextureAttachement::Color:
            *_usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment;
            *_finalLoayout = vk::ImageLayout::eColorAttachmentOptimal;
            *_imageAspectFlag = vk::ImageAspectFlagBits::eColor;

            break;
    case PC_CORE::TextureAttachement::DepthStencil:
            *_usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eDepthStencilAttachment;
            *_finalLoayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            *_imageAspectFlag = vk::ImageAspectFlagBits::eDepth;
            break;
        }
        
        break;
    }

    if (_createTextureInfo.canbeSampled)
        *_usage |= vk::ImageUsageFlagBits::eSampled;

    if (_createTextureInfo.GenerateMipMap)
        *_usage |= vk::ImageUsageFlagBits::eTransferSrc;
}

void Vulkan::GenerateMipMap(vk::CommandBuffer _commandBuffer, vk::Image image, vk::ImageAspectFlags aspectFlag,
                            vk::Format format, int32_t imageWidth, int32_t imageHeight, uint32_t _mipLevel)
{
    //VkFormatProperties formatProperties;
     //vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

     //if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
       //  throw std::runtime_error("texture image format does not support linear blitting!");
     //}

     vk::ImageMemoryBarrier barrier{};
     barrier.sType = vk::StructureType::eImageMemoryBarrier;
     barrier.image = image;
     barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
     barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
     barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
     barrier.subresourceRange.baseArrayLayer = 0;
     barrier.subresourceRange.layerCount = 1;
     barrier.subresourceRange.levelCount = 1;

     int32_t mipWidth = imageWidth;
     int32_t mipHeight = imageHeight;

     for (uint32_t i = 1; i < _mipLevel; i++) 
     {
         barrier.subresourceRange.baseMipLevel = i - 1;
         barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
         barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
         barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
         barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

         vk::DependencyFlags depencyFlag{};
         _commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, depencyFlag,
             0, nullptr,
             0, nullptr,
             1, &barrier);
      
         vk::ImageBlit blit{};
         blit.srcOffsets[0] = vk::Offset3D({ 0, 0, 0});
         blit.srcOffsets[1] = vk::Offset3D({ mipWidth, mipHeight, 1 }) ;
         blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
         blit.srcSubresource.mipLevel = i - 1;
         blit.srcSubresource.baseArrayLayer = 0;
         blit.srcSubresource.layerCount = 1;
         blit.dstOffsets[0] = vk::Offset3D{ 0, 0, 0 };
         blit.dstOffsets[1] = vk::Offset3D{ mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
         blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
         blit.dstSubresource.mipLevel = i;
         blit.dstSubresource.baseArrayLayer = 0;
         blit.dstSubresource.layerCount = 1;

         _commandBuffer.blitImage(
             image, vk::ImageLayout::eTransferSrcOptimal,
             image, vk::ImageLayout::eTransferDstOptimal,
             1, &blit,
             vk::Filter::eLinear);

         barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
         barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
         barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
         barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

         vk::DependencyFlags innerLoopDepencyFlag{};
         _commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, innerLoopDepencyFlag,
             0, nullptr,
             0, nullptr,
             1, &barrier);

         if (mipWidth > 1) mipWidth /= 2;
         if (mipHeight > 1) mipHeight /= 2;
     }

     barrier.subresourceRange.baseMipLevel = _mipLevel - 1;
     barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
     barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
     barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
     barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

     vk::DependencyFlags depencyFlag{};
     _commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, depencyFlag,
         0, nullptr,
         0, nullptr,
         1, &barrier);
 
}
