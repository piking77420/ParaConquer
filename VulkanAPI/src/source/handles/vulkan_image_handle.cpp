
#include "handles/vulkan_image_handle.hpp"

#include "helper_functions.hpp"
#include "rhi_vulkan_parser.hpp"
#include "transition_image_layout.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_context.hpp"
#include "vulkan_image.hpp"
#include "low_renderer/rhi.hpp"


static int GetMultiplayer(PC_CORE::Channel _channel)
{
    switch (_channel)
    {
    case PC_CORE::Channel::GREY:
    case PC_CORE::Channel::ALPHA:
        return 1;
        break;
    case PC_CORE::Channel::RGB:
        return 3; 
        break;
    case PC_CORE::Channel::RGBA:
        return  4;
        break;
    case PC_CORE::Channel::DEFAULT:
    default: ;
    }

    throw std::runtime_error("Vulkan::VulkanGpuAllocator::GetMultiplayer: Invalid Channel");
}



Vulkan::VulkanImageHandle::VulkanImageHandle(const PC_CORE::CreateImageInfo& _createTextureInfo)
{
    auto& context = VulkanContext::GetContext();
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->GetDevice();

    vk::ImageUsageFlags textureUsage;
    VmaMemoryUsage textureMemoryUsage;
    vk::ImageLayout finalTextureLayout = vk::ImageLayout::eUndefined;
    vk::ImageAspectFlags imageAspectFlag;
    uint32_t mipLevel = _createTextureInfo.GenerateMipMap ? _createTextureInfo.mipsLevels : 1;
    GetTextureUsage(_createTextureInfo,&textureMemoryUsage, &textureUsage, &finalTextureLayout, &imageAspectFlag);
    const vk::Format format = RHIFormatToVkFormat(_createTextureInfo.format);
    vk::SampleCountFlagBits sampleCount = RhiSampleCountToVuklan(_createTextureInfo.samples);

    
    // create buffer and alloc
   CreateImage(context.allocator, _createTextureInfo.width
               ,_createTextureInfo.height, _createTextureInfo.depth, _createTextureInfo.mipsLevels,
               sampleCount,
               RHIImageToVkImageType(_createTextureInfo.imageType),
               format, vk::ImageTiling::eOptimal, textureUsage, textureMemoryUsage, &m_VulkanImage, &m_VmaAllocation);

    const SingleCommandBeginInfo singleCommandBeginInfo =
    {
        .device = device,
        .commandPool = context.transferCommandPool,
        .queue = context.transferQueu
    };

    vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);
    
    if (_createTextureInfo.textureNature == PC_CORE::TextureNature::Default && _createTextureInfo.data != nullptr)
    {
        int multiplayer = GetMultiplayer(_createTextureInfo.channel);
    
        if (_createTextureInfo.depth < 1 )
        {
            PC_LOGERROR("Vulkan::VulkanGpuAllocator::CreateTexture: _createTextureInfo.depth < 1 )");
        }
    
        size_t imageSize = static_cast<size_t>(_createTextureInfo.width * _createTextureInfo.height * _createTextureInfo.depth * multiplayer);

        VmaAllocation stagingBufferAlloc = VK_NULL_HANDLE;
        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        
        
        CreateBuffer(context.allocator, imageSize, vk::BufferUsageFlagBits::eTransferSrc, VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU,
            &stagingBuffer, &stagingBufferAlloc);

        void* data = nullptr;
        vmaMapMemory(context.allocator, stagingBufferAlloc, &data);
        memcpy(data, _createTextureInfo.data, imageSize);
        vmaUnmapMemory(context.allocator, stagingBufferAlloc);
        
        // Wait for end command 
        TransitionImageLayout(commandBuffer,  m_VulkanImage, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, imageAspectFlag, mipLevel);
        EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);


        const uint32_t w =  _createTextureInfo.width;
        const uint32_t h =  _createTextureInfo.height;
        const uint32_t d =  _createTextureInfo.depth;
    
        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = imageAspectFlag;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = VkOffset3D{0, 0, 0};
        region.imageExtent = vk::Extent3D{
            w,
            h,
            d
        };
        
        commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);
        commandBuffer.copyBufferToImage(stagingBuffer, m_VulkanImage, vk::ImageLayout::eTransferDstOptimal, region );
        EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);

        // usless buffer now
        DestroyBuffer(device, context.allocator, stagingBuffer, stagingBufferAlloc);

        commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);

        if (mipLevel != 1)
        {
            //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
            GenerateMipMap(commandBuffer, m_VulkanImage, imageAspectFlag, format, _createTextureInfo.width, _createTextureInfo.height, mipLevel);
        }
        else
        {
            TransitionImageLayout(commandBuffer, m_VulkanImage, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, imageAspectFlag, mipLevel);
        }
        EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);

    }
    else
    {
        TransitionImageLayout(commandBuffer,  m_VulkanImage, format, vk::ImageLayout::eUndefined, finalTextureLayout, imageAspectFlag, mipLevel);
        EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);
    }


    m_VulkanImageView = CreateImageView(device, m_VulkanImage, vk::ImageViewType::e2D, format, imageAspectFlag, mipLevel);
}

Vulkan::VulkanImageHandle::~VulkanImageHandle()
{
    if (m_VulkanImage == VK_NULL_HANDLE || m_VulkanImageView == VK_NULL_HANDLE || m_VmaAllocation == VK_NULL_HANDLE)
        return;

    auto& context = VulkanContext::GetContext();
    
    vmaDestroyImage(  context.allocator, m_VulkanImage, m_VmaAllocation);
    context.GetDevice()->GetDevice().destroyImageView(m_VulkanImageView);

    m_VulkanImageView = VK_NULL_HANDLE;
    m_VulkanImage = VK_NULL_HANDLE;
    m_VmaAllocation = VK_NULL_HANDLE;
}

