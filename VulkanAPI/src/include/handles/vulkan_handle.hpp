#pragma once

#include <vma/vk_mem_alloc.h>

#include "vulkan_header.h"
#include "low_renderer/gpu_resource.hpp"

namespace Vulkan
{
    struct VulkanHandle : public PC_CORE::GPUResource
    {
        VulkanHandle() = default;

        ~VulkanHandle() override  = default;
        
    protected:
        VmaAllocation m_VmaAllocation = VK_NULL_HANDLE;

    };
}