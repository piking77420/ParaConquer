
#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include "vulkan_memory_allocator.hpp"


Vulkan::VulkanMemoryAllocator::VulkanMemoryAllocator(const VmaAllocatorCreateInfo& _vmaAllocatorCreateInfo)
{
    
}

Vulkan::VulkanMemoryAllocator::~VulkanMemoryAllocator()
{
    if (m_Allocator != VK_NULL_HANDLE)
    {
        vmaDestroyAllocator(m_Allocator);
        m_Allocator = VK_NULL_HANDLE;
    }
}
