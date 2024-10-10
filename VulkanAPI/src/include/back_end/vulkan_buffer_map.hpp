#pragma once

#include "vulkan_header.h"
#include "render_harware_interface/command_pool.hpp"

namespace VK_NP
{
    struct BufferInternal
    {
        size_t size;
        VmaAllocation allocation;
        PC_CORE::GPU_BUFFER_USAGE usage;
    };

    class VulkanBufferMap
    {
    public:
        using BufferKeyHandle = PC_CORE::GPUBufferHandle;
        
        BufferKeyHandle CreateBuffer(VulkanContext* _context, vk::CommandPool _commandPool, uint32_t _size, const void* data, PC_CORE::GPU_BUFFER_USAGE usage);
        
        const BufferInternal& GetBufferUsage(BufferKeyHandle _bufferKeyHandle);

        bool DestroyBuffer(VulkanContext* _context, BufferKeyHandle _bufferKeyHandle);
        
        void Init(VulkanContext* _vulkanContext);

        void Destroy(VulkanContext* _vulkanContext);
        
    private:
        
        vk::BufferUsageFlags GetVulkanUsage(PC_CORE::GPU_BUFFER_USAGE usage);
        
        std::unordered_map<BufferKeyHandle, BufferInternal> m_BuffersMap;
        
    };

}

