#pragma once
#include "vulkan_frame_buffer.hpp"
#include "buffer/vulkan_buffer_handle.hpp"
#include "low_renderer/gpu_allocator.hpp"

namespace Vulkan
{
    class VulkanGpuAllocator : public PC_CORE::GpuAllocator
    {
    public:
        bool CreateVulkanBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<PC_CORE::GpuBufferHandle>* _vulkanBufferPtr);
        
        bool DestroyBuffer(std::shared_ptr<PC_CORE::GpuBufferHandle>* _vulkanBufferPtr);
        
        VulkanGpuAllocator(const VmaAllocatorCreateInfo& _createInfo);
        
        VulkanGpuAllocator() = default;
        
        ~VulkanGpuAllocator();

    private:
        
        
        VmaAllocator m_allocator;
    };

}

