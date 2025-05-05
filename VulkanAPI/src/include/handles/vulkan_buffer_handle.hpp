#pragma once

#include <vma/vk_mem_alloc.h>

#include "vulkan_header.h"
#include "low_renderer/gpu_handle.hpp"

namespace Vulkan
{

    class VulkanBufferHandle : public PC_CORE::GpuHandle
    {
    public:

        VulkanBufferHandle(VulkanBufferHandle&& _other) noexcept
        {
            std::exchange(buffer,_other.buffer);
            
            std::exchange(allocation, _other.allocation);
        }

        VulkanBufferHandle& operator=(VulkanBufferHandle&& _other)
        {
            std::exchange(buffer,_other.buffer);
            
            std::exchange(allocation, _other.allocation);

            return *this;
        }

        VulkanBufferHandle() = default;

        ~VulkanBufferHandle() override = default;
        
        VkBuffer buffer;
        VmaAllocation allocation;
    };
    
}
