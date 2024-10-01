#pragma once

#include "vulkan_header.h"

namespace VK_NP
{
    struct BufferInternal
    {
        size_t size;
        PC_CORE::GPU_BUFFER_USAGE usage;
            
        vk::Buffer bufferHandle;
        VmaAllocation allocation;
    };
    // This class is responsable for creating buffer and destroying it
    // should bind this buffer tho
    class VulkanBufferMap
    {
    public:
        
        uint32_t CreateBuffer(uint32_t _size, const void* data, PC_CORE::GPU_BUFFER_USAGE usage);
        
        const BufferInternal& GetBufferUsage(uint32_t _bufferId, PC_CORE::GPU_BUFFER_USAGE _bufferUsage);

        void DestroyBuffer(uint32_t _bufferId);

        VulkanBufferMap();

        ~VulkanBufferMap();
    private:
        
        void CopyDataToBuffer(BufferInternal* _bufferInternal, const void* _data);
        
        vk::BufferUsageFlags GetVulkanUsage(PC_CORE::GPU_BUFFER_USAGE usage);
        
        std::unordered_map<uint32_t, BufferInternal> m_BuffersMap;
        
    };

}

