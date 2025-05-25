#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "vulkan_gpu_resource_allocator.hpp"

#include "helper_functions.hpp"
#include "rhi_vulkan_parser.hpp"
#include "transition_image_layout.hpp"
#include "vulkan_context.hpp"
#include "vulkan_device.hpp"
#include "low_renderer/rhi.hpp"


bool Vulkan::VulkanGpuAllocator::CreateGPUBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<PC_CORE::GPUResource>* _bufferptr)
{
    vk::BufferUsageFlags bufferUsageFlags;
    
    try
    {
        bufferUsageFlags = GetVulkanBufferUsageFlagsClient(_createInfo.usage);
    }
    catch (...)
    {
        return false;
    }
    
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::make_shared<VulkanBufferHandle>();
    // Create Client Buffer
    *vulkanBufferPtr = CreateBuffer(_createInfo.dataSize, bufferUsageFlags, GetVmaMemoryUsage(_createInfo.usage));
    
    switch (_createInfo.usage)
    {
    case PC_CORE::BufferUsage::VertexBuffer:
    case PC_CORE::BufferUsage::IndexBuffer:
        {
            VulkanBufferHandle stagginBuffer = CreateBuffer(_createInfo.dataSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_TO_GPU);
            VulkanContext& context = *reinterpret_cast<VulkanContext*>(PC_CORE::Rhi::GetRhiContext());

            SingleCommandBeginInfo singleCommandBeginInfo =
            {
                .device = context.GetDevice()->GetDevice(),
                .commandPool = context.transferCommandPool,
                .queue = context.transferQueu
                };

            vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);
            
            void* data = nullptr;
            vmaMapMemory(m_allocator, stagginBuffer.allocation, &data);
            memcpy(data, _createInfo.data, _createInfo.dataSize);

            vk::BufferCopy copyRegion = {};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = _createInfo.dataSize;
            
            commandBuffer.copyBuffer(stagginBuffer.buffer, vulkanBufferPtr->buffer, copyRegion);
            vmaUnmapMemory(m_allocator, stagginBuffer.allocation);

            EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo);
        }
        break;
    case PC_CORE::BufferUsage::UniformBuffer:
    case PC_CORE::BufferUsage::ShaderStorageBuffer:
        break;
    case PC_CORE::BufferUsage::Count:
    default:
        throw std::invalid_argument("Invalid buffer usage");
    }



    *_bufferptr = vulkanBufferPtr;
    
    return true;
}

bool Vulkan::VulkanGpuAllocator::DestroyBuffer(PC_CORE::GPUResource* _bufferptr)
{
    VulkanBufferHandle* vulkanBufferPtr = reinterpret_cast<VulkanBufferHandle*>(_bufferptr);
    
    if (vulkanBufferPtr->allocation == VK_NULL_HANDLE || vulkanBufferPtr->buffer == VK_NULL_HANDLE)
        return false;
    
    vmaDestroyBuffer(m_allocator, vulkanBufferPtr->buffer, vulkanBufferPtr->allocation);
    
    return true;
}

bool Vulkan::VulkanGpuAllocator::MapBuffer(const std::shared_ptr<PC_CORE::GPUResource>& _bufferptr, void** _mapPtr)
{
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::reinterpret_pointer_cast<VulkanBufferHandle>(_bufferptr);

    if (vulkanBufferPtr->allocation == VK_NULL_HANDLE || vulkanBufferPtr->buffer == VK_NULL_HANDLE)
        return false;

    vmaMapMemory(m_allocator, vulkanBufferPtr->allocation, _mapPtr);

    return true;
}

bool Vulkan::VulkanGpuAllocator::UnMapBuffer(const std::shared_ptr<PC_CORE::GPUResource>& _bufferptr)
{
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::reinterpret_pointer_cast<VulkanBufferHandle>(_bufferptr);

    if (vulkanBufferPtr->allocation == VK_NULL_HANDLE || vulkanBufferPtr->buffer == VK_NULL_HANDLE)
        return false;

    vmaUnmapMemory(m_allocator,vulkanBufferPtr->allocation);
    return true;
}

bool Vulkan::VulkanGpuAllocator::CreateTexture(const PC_CORE::CreateTextureInfo& _createTextureInfo,
    std::shared_ptr<PC_CORE::GPUResource>* _texturePtr)
{
    VulkanContext& context = *reinterpret_cast<VulkanContext*>(PC_CORE::Rhi::GetRhiContext());
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->GetDevice();

    vk::ImageUsageFlags textureUsage;
    VmaMemoryUsage textureMemoryUsage;
    vk::ImageLayout finalTextureLayout = vk::ImageLayout::eUndefined;
    vk::ImageAspectFlags imageAspectFlag;

    uint32_t mipLevel = _createTextureInfo.GenerateMipMap ? _createTextureInfo.mipsLevels : 1;
    
    GetTextureUsage(_createTextureInfo,&textureMemoryUsage, &textureUsage, &finalTextureLayout, &imageAspectFlag);
    const vk::Format format = RHIFormatToVkFormat(_createTextureInfo.format);
    
    VulkanImageHandle vulkanImageHandle = CreateImage(_createTextureInfo.width, _createTextureInfo.height
                                                     ,_createTextureInfo.depth, _createTextureInfo.mipsLevels,
                                                     RHIImageToVkImageType(_createTextureInfo.imageType),
                                                     format,
                                                     vk::ImageTiling::eOptimal, textureUsage, textureMemoryUsage);

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
            return false;
        }
    
        size_t imageSize = static_cast<size_t>(_createTextureInfo.width * _createTextureInfo.height * _createTextureInfo.depth * multiplayer);
    
        VulkanBufferHandle stagginBuffer = CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU);

        void* data = nullptr;
        vmaMapMemory(m_allocator, stagginBuffer.allocation, &data);
        memcpy(data, _createTextureInfo.data, imageSize);
        vmaUnmapMemory(m_allocator, stagginBuffer.allocation);
        
        // Wait for end command 
        TransitionImageLayout(commandBuffer,  vulkanImageHandle.image, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, imageAspectFlag, mipLevel);
        EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo);


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
        commandBuffer.copyBufferToImage(stagginBuffer.buffer, vulkanImageHandle.image, vk::ImageLayout::eTransferDstOptimal, region );
        EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo);

        commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);

        if (mipLevel != 1)
        {
            GenerateMipMap(commandBuffer, vulkanImageHandle.image, imageAspectFlag, format, _createTextureInfo.width, _createTextureInfo.height, mipLevel);
            //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
        }
        else
        {
            TransitionImageLayout(commandBuffer, vulkanImageHandle.image, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, imageAspectFlag, mipLevel);
        }
        EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo);

    }
    else
    {
        TransitionImageLayout(commandBuffer,  vulkanImageHandle.image, format, vk::ImageLayout::eUndefined, finalTextureLayout, imageAspectFlag, mipLevel);
        EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo);
    }


    vulkanImageHandle.view = CreateImageView(device, vulkanImageHandle.image, vk::ImageViewType::e2D,
    format, imageAspectFlag, mipLevel);
    
    *_texturePtr = std::make_shared<VulkanImageHandle>(std::move(vulkanImageHandle));
  
    vulkanImageHandle.Clear();
}

bool Vulkan::VulkanGpuAllocator::DestroyImage(PC_CORE::GPUResource* _textureHandle)
{
    VulkanImageHandle* vulkanBufferPtr = reinterpret_cast<VulkanImageHandle*>(_textureHandle);
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->GetDevice();

    if (vulkanBufferPtr->allocation == VK_NULL_HANDLE || vulkanBufferPtr->image == VK_NULL_HANDLE || vulkanBufferPtr->view == VK_NULL_HANDLE)
        return false;
    
    vmaDestroyImage(m_allocator, vulkanBufferPtr->image, vulkanBufferPtr->allocation);
    device.destroyImageView(vulkanBufferPtr->view);
       
 
    return true;
}

Vulkan::VulkanGpuAllocator::VulkanGpuAllocator(const VmaAllocatorCreateInfo& _createInfo)
{
    vmaCreateAllocator(&_createInfo ,&m_allocator);
}

Vulkan::VulkanGpuAllocator::~VulkanGpuAllocator()
{
    if (m_allocator == VK_NULL_HANDLE)
        return;

    vmaDestroyAllocator(m_allocator);
}

VmaMemoryUsage Vulkan::VulkanGpuAllocator::GetVmaMemoryUsage(PC_CORE::BufferUsage bufferUsage)
{
    switch (bufferUsage)
    {
    case PC_CORE::BufferUsage::VertexBuffer:
    case PC_CORE::BufferUsage::IndexBuffer:
        return VMA_MEMORY_USAGE_GPU_ONLY;
    case PC_CORE::BufferUsage::UniformBuffer:
        return VMA_MEMORY_USAGE_CPU_TO_GPU;
    case PC_CORE::BufferUsage::ShaderStorageBuffer:
        return static_cast<VmaMemoryUsage>(VMA_MEMORY_USAGE_CPU_TO_GPU | VMA_MEMORY_USAGE_GPU_TO_CPU);
    case PC_CORE::BufferUsage::Count:
        break;
    default: ;
    }
}

vk::BufferUsageFlags Vulkan::VulkanGpuAllocator::GetVulkanBufferUsageFlagsClient(PC_CORE::BufferUsage bufferUsage)
{
    vk::BufferUsageFlags based = RhiToBufferUsage(bufferUsage);
    
    switch (bufferUsage)
    {
    case PC_CORE::BufferUsage::VertexBuffer:
    case PC_CORE::BufferUsage::IndexBuffer:
        return based | vk::BufferUsageFlagBits::eTransferDst;
    case PC_CORE::BufferUsage::UniformBuffer:
    case PC_CORE::BufferUsage::ShaderStorageBuffer:
        return based;
    case PC_CORE::BufferUsage::Count:
        break;
    default: ;
    }

}

int Vulkan::VulkanGpuAllocator::GetMultiplayer(PC_CORE::Channel _channel)
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




Vulkan::VulkanBufferHandle Vulkan::VulkanGpuAllocator::CreateBuffer(size_t size,
    vk::BufferUsageFlags _bufferUsageFlagBits, VmaMemoryUsage _memoryUsage)
{
       
    vk::BufferCreateInfo bufferCreate{};
    bufferCreate.sType = vk::StructureType::eBufferCreateInfo;
    bufferCreate.size = static_cast<VkDeviceSize>(size);
    bufferCreate.usage = _bufferUsageFlagBits;
    bufferCreate.sharingMode = vk::SharingMode::eExclusive;

    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = _memoryUsage;
    
    VulkanBufferHandle vulkanBufferHandle;
    
    VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(m_allocator, reinterpret_cast<VkBufferCreateInfo*>(&
                        bufferCreate), &allocationInfo, &vulkanBufferHandle.buffer, &vulkanBufferHandle.allocation, nullptr)));

    return vulkanBufferHandle;
}

Vulkan::VulkanImageHandle Vulkan::VulkanGpuAllocator::CreateImage(uint32_t width, uint32_t height, uint32_t depth,
                                                                  uint32_t _mimpLevel,
                                                                  vk::ImageType _imageType,
                                                                  vk::Format format, vk::ImageTiling tiling,
                                                                  vk::ImageUsageFlags usage, VmaMemoryUsage imageMemory)
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

    
    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = imageMemory;
    
    Vulkan::VulkanImageHandle vulkanImageHandle;
    vmaCreateImage(m_allocator, reinterpret_cast<VkImageCreateInfo*>(&imageInfo), &allocationInfo,&vulkanImageHandle.image, &vulkanImageHandle.allocation, nullptr);
    return vulkanImageHandle;
}

vk::ImageView Vulkan::VulkanGpuAllocator::CreateImageView(vk::Device _device, vk::Image _image, vk::ImageViewType _imageType,
    vk::Format _format, vk::ImageAspectFlags imageAspect, uint32_t _mipLevels)
{

    vk::ImageViewCreateInfo imageInfo{};
    imageInfo.sType  = vk::StructureType::eImageViewCreateInfo;
    imageInfo.image = _image;
    imageInfo.viewType = vk::ImageViewType::e2D;
    imageInfo.format = _format;
    imageInfo.subresourceRange.aspectMask = imageAspect;
    imageInfo.subresourceRange.baseMipLevel = 0;
    imageInfo.subresourceRange.levelCount = 1;
    imageInfo.subresourceRange.baseArrayLayer = 0;
    imageInfo.subresourceRange.layerCount = 1;
    imageInfo.subresourceRange.levelCount = _mipLevels;

    vk::ImageView imageView;

    VK_CALL(_device.createImageView(&imageInfo, nullptr, &imageView));

    return imageView;
}

 void Vulkan::VulkanGpuAllocator::GetTextureUsage(const PC_CORE::CreateTextureInfo& _createTextureInfo, VmaMemoryUsage* _memoryUsage, vk::ImageUsageFlags* _usage , vk::ImageLayout* _finalLoayout, vk::ImageAspectFlags* _imageAspectFlag)
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

 void Vulkan::VulkanGpuAllocator::GenerateMipMap(vk::CommandBuffer _commandBuffer, vk::Image image, vk::ImageAspectFlags _imageAspectFlag ,vk::Format format, int32_t imageWidth, int32_t imageHeight, uint32_t _mipLevel)
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

