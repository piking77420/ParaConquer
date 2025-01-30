#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "vulkan_gpu_allocator.hpp"

#include "helper_functions.hpp"
#include "rhi_vulkan_parser.hpp"
#include "vulkan_context.hpp"
#include "vulkan_device.hpp"
#include "low_renderer/rhi.hpp"


bool Vulkan::VulkanGpuAllocator::CreateGPUBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<PC_CORE::GpuBufferHandle>* _bufferptr)
{
    vk::BufferCreateInfo vkClientBufferCreateInfo{};
    try
    {
        vkClientBufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
        vkClientBufferCreateInfo.size = _createInfo.dataSize;
        vkClientBufferCreateInfo.usage = GetVulkanBufferUsageFlagsClient(_createInfo.usage);
        vkClientBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    }
    catch (...)
    {
        return false;
    }
   

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = GetVmaMemoryUsage(_createInfo.usage);
    
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::make_shared<VulkanBufferHandle>();
    // Create Client Buffer
    VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(m_allocator, reinterpret_cast<VkBufferCreateInfo*>(&vkClientBufferCreateInfo), &vmaallocInfo,
    &vulkanBufferPtr->buffer,
    &vulkanBufferPtr->allocation,
    nullptr)));

    
    switch (_createInfo.usage)
    {
    case PC_CORE::BufferUsage::VertexBuffer:
    case PC_CORE::BufferUsage::IndexBuffer:
        {
            
            VulkanBufferHandle stagginBuffer;
            VmaAllocationCreateInfo allocationInfo = {};
            allocationInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            
            vk::BufferCreateInfo vkStaginBufferCreateInfo{};
            vkStaginBufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
            vkStaginBufferCreateInfo.size = _createInfo.dataSize;
            vkStaginBufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
            vkStaginBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

            VK_CALL(
                static_cast<vk::Result>(vmaCreateBuffer(m_allocator, reinterpret_cast<VkBufferCreateInfo*>(&
                        vkStaginBufferCreateInfo), &allocationInfo,
                    &stagginBuffer.buffer,
                    &stagginBuffer.allocation,
                    nullptr)));

            void* data = nullptr;
            vmaMapMemory(m_allocator, stagginBuffer.allocation, &data);
            memcpy(data, _createInfo.data, _createInfo.dataSize);
            CopyBuffer(stagginBuffer.buffer, vulkanBufferPtr->buffer, vkStaginBufferCreateInfo.size);
            vmaUnmapMemory(m_allocator, stagginBuffer.allocation);

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

bool Vulkan::VulkanGpuAllocator::DestroyBuffer(std::shared_ptr<PC_CORE::GpuBufferHandle>* _bufferptr)
{
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::reinterpret_pointer_cast<VulkanBufferHandle>(*_bufferptr);
    
    if (vulkanBufferPtr->allocation == VK_NULL_HANDLE || vulkanBufferPtr->buffer == VK_NULL_HANDLE)
        return false;
    
    vmaDestroyBuffer(m_allocator, vulkanBufferPtr->buffer, vulkanBufferPtr->allocation);
    
    return true;
}

bool Vulkan::VulkanGpuAllocator::MapBuffer(const std::shared_ptr<PC_CORE::GpuBufferHandle>& _bufferptr, void** _mapPtr)
{
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::reinterpret_pointer_cast<VulkanBufferHandle>(_bufferptr);

    if (vulkanBufferPtr->allocation == VK_NULL_HANDLE || vulkanBufferPtr->buffer == VK_NULL_HANDLE)
        return false;

    vmaMapMemory(m_allocator, vulkanBufferPtr->allocation, _mapPtr);

    return true;
}

bool Vulkan::VulkanGpuAllocator::UnMapBuffer(const std::shared_ptr<PC_CORE::GpuBufferHandle>& _bufferptr)
{
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::reinterpret_pointer_cast<VulkanBufferHandle>(_bufferptr);

    if (vulkanBufferPtr->allocation == VK_NULL_HANDLE || vulkanBufferPtr->buffer == VK_NULL_HANDLE)
        return false;

    vmaUnmapMemory(m_allocator,vulkanBufferPtr->allocation);
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

void Vulkan::VulkanGpuAllocator::CopyBuffer(vk::Buffer _src, vk::Buffer _dst, vk::DeviceSize _size)
{
    VulkanContext& context = *reinterpret_cast<VulkanContext*>(PC_CORE::Rhi::GetRhiContext());

    SingleCommandBeginInfo singleCommandBeginInfo =
        {
        .device = context.GetDevice()->GetDevice(),
        .commandPool = context.transferCommandPool,
        .queue = context.transferQueu
        };
    
    vk::CommandBuffer copyCommand = BeginSingleTimeCommand(singleCommandBeginInfo);

    vk::BufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = _size;
    
    copyCommand.copyBuffer(_src, _dst, copyRegion);

    EndSingleTimeCommand(copyCommand, singleCommandBeginInfo);
    
}

