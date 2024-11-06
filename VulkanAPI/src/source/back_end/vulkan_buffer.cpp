#include "back_end/vulkan_buffer.hpp"

#include "rhi_vulkan_parser.hpp"
#include <vma/vk_mem_alloc.h>

#include "back_end/vulkan_command_pool_function.hpp"

void Vulkan::Backend::CreateBuffer(VulkanContext* _vulkanContext, size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage,
   vk::Buffer* _outBuffer, VmaAllocation* _outVmaAllocation)
{
    vk::Buffer buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VmaAllocationInfo allocationInfo = {};
    const uint32_t size = static_cast<uint32_t>(_size);
    vk::BufferUsageFlags bufferUsageFlags = GetVulkanUsage(_usage);

    switch (_usage)
    {
        break;
    case PC_CORE::GPU_BUFFER_USAGE::VERTEX:
        Backend::CreateGPUBufferFromCPU(_vulkanContext, _vulkanContext->resourceCommandPool, size, _data, bufferUsageFlags,
                                        &buffer, &allocation);

    case PC_CORE::GPU_BUFFER_USAGE::INDEX:
        Backend::CreateGPUBufferFromCPU(_vulkanContext, _vulkanContext->resourceCommandPool, size, _data, bufferUsageFlags,
                                        &buffer, &allocation);

    case PC_CORE::GPU_BUFFER_USAGE::DYNAMIC_UNIFORM:
    case PC_CORE::GPU_BUFFER_USAGE::UNIFORM:
        Backend::CreateBufferAndAlloc(_vulkanContext, size, bufferUsageFlags,
                                     VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
                                     VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                                     &buffer, &allocation, &allocationInfo);
        break;
    case PC_CORE::GPU_BUFFER_USAGE::SHADER_STORAGE:
        break;

    case PC_CORE::GPU_BUFFER_USAGE::TRANSFERT_SRC:
        Backend::CreateBufferAndAlloc(_vulkanContext, size, bufferUsageFlags,
            VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
            VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
            &buffer, &allocation, &allocationInfo);
        break;
    case PC_CORE::GPU_BUFFER_USAGE::NONE:
    case PC_CORE::GPU_BUFFER_USAGE::COUNT:
        break;
    }

    *_outBuffer = buffer;
    *_outVmaAllocation = allocation;
}

void Vulkan::Backend::CreateBufferAndAlloc(VulkanContext* _context, uint32_t _size,
                                           vk::BufferUsageFlags _vkBufferUsageFlags, VmaMemoryUsage vmaMemoryUsage,
                                           VmaAllocationCreateFlags vmaAllocationCreateFlagBits, vk::Buffer* _buffer, VmaAllocation* _allocation,
                                           VmaAllocationInfo* _allocationInfo)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = _size;
    bufferCreateInfo.usage = static_cast<VkBufferUsageFlags>(_vkBufferUsageFlags);
    bufferCreateInfo.flags = 0;
    
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // Or VK_SHARING_MODE_CONCURRENT if needed

    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = vmaMemoryUsage;
    allocationInfo.flags = vmaAllocationCreateFlagBits;

    VkBuffer vkBuffer = VK_NULL_HANDLE; 

    VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(_context->allocator, &bufferCreateInfo, &allocationInfo, &vkBuffer, _allocation,
        _allocationInfo)));

    *_buffer = vkBuffer;
}

void  Vulkan::Backend::CreateGPUBufferFromCPU(VulkanContext* _context, vk::CommandPool _commandPool, uint32_t _size, const void* _data, vk::BufferUsageFlags _vkBufferUsageFlags,
        vk::Buffer* _vkBuffer, VmaAllocation* _vmaAllocation)
{
     vk::Buffer stagingNuffer = VK_NULL_HANDLE;
    VmaAllocation vmaAllocationStaging = {};
    VmaAllocationInfo stagingBufferAllocationInfo = {};

    
    CreateBufferAndAlloc(_context, _size, _vkBufferUsageFlags | vk::BufferUsageFlagBits::eTransferSrc,
        VMA_MEMORY_USAGE_CPU_TO_GPU,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        &stagingNuffer, &vmaAllocationStaging, &stagingBufferAllocationInfo);

    void* mappedData = nullptr;
    vmaMapMemory(_context->allocator, vmaAllocationStaging, &mappedData);
    memcpy(mappedData, _data, _size);  // Copy data to the staging buffer

    vk::Buffer VertexBufferNuffer = VK_NULL_HANDLE;
    VmaAllocation vmaAllocationVertex = {};
    VmaAllocationInfo VertexBufferAllocationInfo = {};

    CreateBufferAndAlloc(_context, _size, _vkBufferUsageFlags | vk::BufferUsageFlagBits::eTransferDst,
        VMA_MEMORY_USAGE_GPU_ONLY,
        0,  // No flags for GPU-only memory
        &VertexBufferNuffer, &vmaAllocationVertex, &VertexBufferAllocationInfo);

    // Allocate and begin the command buffer
    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(_context->device, _commandPool);
    
    // Copy from staging to vertex buffer
    vk::BufferCopy copyRegion = {};
    copyRegion.size = _size;
    commandBuffer.copyBuffer(stagingNuffer, VertexBufferNuffer, 1, &copyRegion);

    EndSingleTimeCommands(commandBuffer, _context->device, _context->resourceFence, _context->vkQueues.graphicQueue);
    
    // Cleanup staging buffer and unmap
    vmaUnmapMemory(_context->allocator, vmaAllocationStaging);
    vmaDestroyBuffer(_context->allocator, stagingNuffer, vmaAllocationStaging);
    
    *_vmaAllocation = vmaAllocationVertex;
    *_vkBuffer = VertexBufferNuffer;
}
