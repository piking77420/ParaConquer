#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "vulkan_gpu_allocator.hpp"

#include "rhi_vulkan_parser.hpp"


bool Vulkan::VulkanGpuAllocator::CreateVulkanBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<PC_CORE::GpuBufferHandle>* _bufferptr)
{
    
    vk::BufferCreateInfo vbufferCreateInfo{};
    vbufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
    vbufferCreateInfo.size = _createInfo.dataSize;
    vbufferCreateInfo.usage = RhiToBufferUsage(_createInfo.usage);
    vbufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::make_shared<VulkanBufferHandle>();

    VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(m_allocator, reinterpret_cast<VkBufferCreateInfo*>(&vbufferCreateInfo), &vmaallocInfo,
    &vulkanBufferPtr->buffer,
    &vulkanBufferPtr->allocation,
    nullptr)));

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

bool Vulkan::VulkanGpuAllocator::CreateBufferInternal(vk::BufferCreateInfo& _createInfo,
    std::shared_ptr<PC_CORE::GpuBufferHandle>* _bufferptr)
{

    VulkanBufferHandle vulkanBufferHandle{};

    
    switch (_createInfo.usage)
    {
    case PC_CORE::BufferUsage::VertexBuffer:
    case PC_CORE::BufferUsage::IndexBuffer:
        VmaAllocationCreateInfo staaginBufferAllcoationCreateInfo = {};
        staaginBufferAllcoationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        vmaCreateBuffer(m_allocator, reinterpret_cast<VkBufferCreateInfo*>(&_createInfo), &staaginBufferAllcoationCreateInfo, &vulkanBufferHandle.buffer, &vulkanBufferHandle.allocation, nullptr);

        
    case PC_CORE::BufferUsage::Count:
        break;
    }
}
