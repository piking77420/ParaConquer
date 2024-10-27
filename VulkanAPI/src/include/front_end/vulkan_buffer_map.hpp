#pragma once

#include "vulkan_header.h"
#include "rendering/render_harware_interface/command_pool.hpp"

namespace VK_NP
{
    struct BufferInternal
    {
        size_t size;
        VmaAllocation allocation;
        VmaAllocationInfo allocationInfo;
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

        void MapData(VulkanContext* _vulkanContext, BufferKeyHandle bufferKeyHandle, void** _data);
        
        void UnMapData(VulkanContext* _vulkanContext, BufferKeyHandle bufferKeyHandle);

    private:

        std::unordered_map<BufferKeyHandle, BufferInternal> m_BuffersMap;

        vk::BufferUsageFlags GetVulkanUsage(PC_CORE::GPU_BUFFER_USAGE usage);
        
        void CreateBufferAndAlloc(VulkanContext* _context, uint32_t _size, VkBufferUsageFlags _vkBufferUsageFlags,
            VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationCreateFlagBits,
            vk::Buffer* _buffer, VmaAllocation* _allocation , VmaAllocationInfo* _allocationInfo);

        BufferKeyHandle CreateGPUBufferFromCPU(VulkanContext* _context, vk::CommandPool _commandPool, uint32_t _size, const void* data, PC_CORE::GPU_BUFFER_USAGE usage);

        BufferKeyHandle CreateCpuBuffer(VulkanContext* _context, vk::CommandPool _commandPool, uint32_t _size, const void* data, PC_CORE::GPU_BUFFER_USAGE usage);

    };

}

