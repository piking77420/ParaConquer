#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "vulkan_gpu_allocator.hpp"

#include "helper_functions.hpp"
#include "rhi_vulkan_parser.hpp"
#include "transition_image_layout.hpp"
#include "vulkan_context.hpp"
#include "vulkan_device.hpp"
#include "low_renderer/rhi.hpp"


bool Vulkan::VulkanGpuAllocator::CreateGPUBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<PC_CORE::GpuHandle>* _bufferptr)
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
            
            vmaDestroyBuffer(m_allocator,stagginBuffer.buffer, stagginBuffer.allocation);
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

bool Vulkan::VulkanGpuAllocator::DestroyBuffer(std::shared_ptr<PC_CORE::GpuHandle>* _bufferptr)
{
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::reinterpret_pointer_cast<VulkanBufferHandle>(*_bufferptr);
    
    if (vulkanBufferPtr->allocation == VK_NULL_HANDLE || vulkanBufferPtr->buffer == VK_NULL_HANDLE)
        return false;
    
    vmaDestroyBuffer(m_allocator, vulkanBufferPtr->buffer, vulkanBufferPtr->allocation);
    
    return true;
}

bool Vulkan::VulkanGpuAllocator::MapBuffer(const std::shared_ptr<PC_CORE::GpuHandle>& _bufferptr, void** _mapPtr)
{
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::reinterpret_pointer_cast<VulkanBufferHandle>(_bufferptr);

    if (vulkanBufferPtr->allocation == VK_NULL_HANDLE || vulkanBufferPtr->buffer == VK_NULL_HANDLE)
        return false;

    vmaMapMemory(m_allocator, vulkanBufferPtr->allocation, _mapPtr);

    return true;
}

bool Vulkan::VulkanGpuAllocator::UnMapBuffer(const std::shared_ptr<PC_CORE::GpuHandle>& _bufferptr)
{
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::reinterpret_pointer_cast<VulkanBufferHandle>(_bufferptr);

    if (vulkanBufferPtr->allocation == VK_NULL_HANDLE || vulkanBufferPtr->buffer == VK_NULL_HANDLE)
        return false;

    vmaUnmapMemory(m_allocator,vulkanBufferPtr->allocation);
    return true;
}

bool Vulkan::VulkanGpuAllocator::CreateTexture(const PC_CORE::CreateTextureInfo& _createTextureInfo,
    std::shared_ptr<PC_CORE::GpuHandle>* _texturePtr)
{
    VulkanContext& context = *reinterpret_cast<VulkanContext*>(PC_CORE::Rhi::GetRhiContext());

    int multiplayer = 0;
    switch (_createTextureInfo.channel)
    {
    case PC_CORE::Channel::GREY:
    case PC_CORE::Channel::ALPHA:
        multiplayer = 1;
        break;
    case PC_CORE::Channel::RGB:
        multiplayer = 3; 
        break;
    case PC_CORE::Channel::RGBA:
        multiplayer = 4;
        break;
    case PC_CORE::Channel::DEFAULT:
    default: ;
    }

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

    const vk::ImageUsageFlags imageUsageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    const VmaMemoryUsage memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
    const vk::Format format = RHIFormatToVkFormat(_createTextureInfo.format);
    
    VulkanImageHandle vulkanImageHandle = CreateImage(_createTextureInfo.width, _createTextureInfo.height
                                                     ,_createTextureInfo.depth, _createTextureInfo.mipsLevels,
                                                     RHIImageToVkImageType(_createTextureInfo.imageType),
                                                     format,
                                                     vk::ImageTiling::eOptimal, imageUsageFlags, memoryUsage);

    const SingleCommandBeginInfo singleCommandBeginInfo =
       {
        .device = context.GetDevice()->GetDevice(),
        .commandPool = context.transferCommandPool,
        .queue = context.transferQueu
        };

    vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);

    TransitionImageLayout(commandBuffer,  vulkanImageHandle.image, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

    const uint32_t w =  _createTextureInfo.width;
    const uint32_t h =  _createTextureInfo.height;
    const uint32_t d =  _createTextureInfo.depth;
    
    vk::BufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = VkOffset3D{0, 0, 0};
    region.imageExtent = vk::Extent3D{
        w,
        h,
        d
    };
    
    commandBuffer.copyBufferToImage(stagginBuffer.buffer, vulkanImageHandle.image,vk::ImageLayout::eTransferDstOptimal,region );
    TransitionImageLayout(commandBuffer,  vulkanImageHandle.image, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo);

    vmaDestroyBuffer(m_allocator, stagginBuffer.buffer, stagginBuffer.allocation);

    std::shared_ptr<VulkanImageHandle> vkPtr = std::make_shared<VulkanImageHandle>();
    
    vkPtr->allocation = vulkanImageHandle.allocation;
    vkPtr->image = vulkanImageHandle.image;

    *_texturePtr = vkPtr;
}

bool Vulkan::VulkanGpuAllocator::DestroyTexture(std::shared_ptr<PC_CORE::GpuHandle>* _textureHandle)
{
    std::shared_ptr<VulkanImageHandle> vulkanBufferPtr = std::reinterpret_pointer_cast<VulkanImageHandle>(*_textureHandle);

    
    if (vulkanBufferPtr->allocation == VK_NULL_HANDLE || vulkanBufferPtr->image == VK_NULL_HANDLE)
        return false;
    
    vmaDestroyImage(m_allocator, vulkanBufferPtr->image, vulkanBufferPtr->allocation);
    *_textureHandle = nullptr;
    
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



void Vulkan::VulkanGpuAllocator::CopyBuffer(vk::CommandBuffer* _commandBuffer, vk::Buffer _src, vk::Buffer _dst, vk::DeviceSize _size)
{
    
    vk::BufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = _size;
    
    
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

