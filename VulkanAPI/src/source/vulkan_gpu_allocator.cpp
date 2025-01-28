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

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    
    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::make_shared<VulkanBufferHandle>();

    VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(m_allocator, reinterpret_cast<VkBufferCreateInfo*>(&vbufferCreateInfo), &vmaallocInfo,
    &vulkanBufferPtr->buffer,
    &vulkanBufferPtr->allocation,
    nullptr)));
    
    return true;
}

bool Vulkan::VulkanGpuAllocator::DestroyBuffer(std::shared_ptr<PC_CORE::GpuBufferHandle>* _bufferptr)
{

    std::shared_ptr<VulkanBufferHandle> vulkanBufferPtr = std::reinterpret_pointer_cast<VulkanBufferHandle>(*_bufferptr);

    vmaDestroyBuffer(m_allocator, vulkanBufferPtr->buffer, vulkanBufferPtr->allocation);
    *_bufferptr = nullptr;
    
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
