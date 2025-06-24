#pragma once

#include <vma/vk_mem_alloc.h>

#include "vulkan_header.h"
#include "low_renderer/rhi_buffer.h"

namespace Vulkan
{
    class VULKAN_API VulkanBuffer : public PC_CORE::RhiBuffer
    {
    public:
        VulkanBuffer();
        
        ~VulkanBuffer() override = default;
    private:
        // TO DO use a vector if data is static for multiple frame
        std::array<vk::Buffer, MAX_FRAMES_IN_FLIGHT> buffers;
        std::array<VmaAllocation, MAX_FRAMES_IN_FLIGHT> allocations;
    };
    
}
