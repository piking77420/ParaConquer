#pragma once

#include <vma/vk_mem_alloc.h>

#include "vulkan_header.h"
#include "low_renderer/gpu_resource.hpp"

namespace Vulkan
{

    class VulkanBufferHandle : public PC_CORE::GPUResource
    {
    public:

        VulkanBufferHandle(VulkanBufferHandle&& _other) noexcept
        {
            std::swap(buffer, _other.buffer);
            std::swap(allocation, _other.allocation);

        }

        VulkanBufferHandle& operator=(VulkanBufferHandle&& _other)
        {
            std::swap(buffer, _other.buffer);
            std::swap(allocation, _other.allocation);

            return *this;
        }

        VulkanBufferHandle();

        ~VulkanBufferHandle() override;

        VULKAN_API void Clear() override;
        
        VkBuffer buffer;
        VmaAllocation allocation;
       
    };
    
}
