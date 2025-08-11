#pragma once

#include <vma/vk_mem_alloc.h>

#include "VulkanHeader.h"
#include "VulkanContext.hpp"
#include "LowRenderer/RhiBuffer.h"


namespace Vulkan
{
    struct BufferAndAlloc
    {
        vk::Buffer buffer = VK_NULL_HANDLE;
        VmaAllocation alloc = VK_NULL_HANDLE;

        DEFAULT_CONSTRUCTOR_DESTRUCTOR(BufferAndAlloc)
        
        DEFAULT_COPY_MOVE_OPERATIONS(BufferAndAlloc)
    };
    
    class VULKAN_API VulkanBuffer
    {
    public:

        DEFAULT_COPY_MOVE_OPERATIONS(VulkanBuffer)

        std::vector<BufferAndAlloc> bufferAndAlloc;

        void MapData(void** _mapData);

        void UnMapData();
        
        VulkanBuffer(const void* _data, uint32_t _sizeInByte, vk::BufferUsageFlags bufferUsage, PC_CORE::MemoryLocalisation _visibility, PC_CORE::MemoryUsage memoryUsage);

        VulkanBuffer(uint32_t _sizeInByte, vk::BufferUsageFlags bufferUsage, PC_CORE::MemoryLocalisation _visibility, PC_CORE::MemoryUsage memoryUsage);

        VulkanBuffer() = default;
        
        ~VulkanBuffer();

    private:
        void CreateInternalBuffer(const void* _data, uint32_t _size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
        
        void SendDataToGPUMemory(const void* _data, uint32_t _size);

    };
    
}
