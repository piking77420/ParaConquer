#pragma once

#include <vma/vk_mem_alloc.h>

#include "vulkan_header.h"
#include "vulkan_context.hpp"
#include "low_renderer/rhi_buffer.h"


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
        
        VulkanBuffer(const void* _data, uint32_t _sizeInByte, vk::BufferUsageFlags bufferUsage, PC_CORE::BufferMemoryUsage memoryUsage);
        
        VulkanBuffer() = default;
        
        ~VulkanBuffer();

    private:
        void CreateStaticBuffer(const void* _data, uint32_t _size, vk::BufferUsageFlags bufferUsage);

        void CreateMutableBuffer(const void* _data, uint32_t _size, vk::BufferUsageFlags bufferUsage);

        void CreateDynamicBuffer(const void* _data, uint32_t _size, vk::BufferUsageFlags bufferUsage);

        void SendDataToGPUMemory(const void* _data, uint32_t _size);

    };
    
}
