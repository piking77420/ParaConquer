#pragma once
#include "vma/vk_mem_alloc.h"

#include "vulkan_header.h"

namespace Vulkan
{
    class VulkanMemoryAllocator
    {
    public:

        VulkanMemoryAllocator(const VmaAllocatorCreateInfo& _vmaAllocatorCreateInfo);

        VulkanMemoryAllocator() = default;
        
        ~VulkanMemoryAllocator();

    private:
        VmaAllocator m_Allocator = VK_NULL_HANDLE;
    };
  
}
