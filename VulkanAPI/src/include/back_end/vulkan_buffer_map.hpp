#pragma once

#include "vulkan_header.h"

namespace VK_NP
{
    struct BufferInternal
    {
        size_t size;
        VmaAllocation allocation;
        PC_CORE::GPU_BUFFER_USAGE usage;
    };
    // This class is responsable for creating buffer and destroying it
    // should bind this buffer tho
    class VulkanBufferMap
    {
    public:
        using BufferKeyHandle = PC_CORE::GPUBufferHandle;
        
        BufferKeyHandle CreateBuffer(uint32_t _size, const void* data, PC_CORE::GPU_BUFFER_USAGE usage);
        
        const BufferInternal& GetBufferUsage(BufferKeyHandle _bufferKeyHandle);

        bool DestroyBuffer(BufferKeyHandle _bufferKeyHandle);

        VulkanBufferMap() = default;

        ~VulkanBufferMap();
    private:
        void CopyDataToBuffer(BufferInternal* _bufferInternal, const void* _data);
        
        vk::BufferUsageFlags GetVulkanUsage(PC_CORE::GPU_BUFFER_USAGE usage);
        
        std::unordered_map<BufferKeyHandle, BufferInternal> m_BuffersMap;
        
    };

}

