#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "vulkan_gpu_allocator.hpp"


bool Vulkan::VulkanGpuAllocator::CreateVulkanBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<PC_CORE::GpuBufferHandle>* _vulkanBufferPtr)
{
    return false;
}

bool Vulkan::VulkanGpuAllocator::DestroyBuffer(std::shared_ptr<PC_CORE::GpuBufferHandle>* _vulkanBufferPtr)
{
    return false;
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
