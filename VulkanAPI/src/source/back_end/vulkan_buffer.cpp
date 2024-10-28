#include "back_end/vulkan_buffer.hpp"

#include "rhi_vulkan_parser.hpp"
#include <vma/vk_mem_alloc.h>

#include "back_end/vulkan_command_pool_function.hpp"

void Vulkan::Backend::CreateBufferAndAlloc(VulkanContext* _context, uint32_t _size,
                                          VkBufferUsageFlags _vkBufferUsageFlags, VmaMemoryUsage vmaMemoryUsage,
                                          VmaAllocationCreateFlags vmaAllocationCreateFlagBits, vk::Buffer* _buffer, VmaAllocation* _allocation,
                                          VmaAllocationInfo* _allocationInfo)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = _size;
    bufferCreateInfo.usage = _vkBufferUsageFlags;
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

void  Vulkan::Backend::CreateGPUBufferFromCPU(VulkanContext* _context, vk::CommandPool _commandPool, uint32_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE usage,
        vk::Buffer* _vkBuffer, VmaAllocation* _vmaAllocation)
{
     vk::Buffer stagingNuffer = VK_NULL_HANDLE;
    VmaAllocation vmaAllocationStaging = {};
    VmaAllocationInfo stagingBufferAllocationInfo = {};

    CreateBufferAndAlloc(_context, _size, static_cast<VkBufferUsageFlags>(GetVulkanUsage(usage)) | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        &stagingNuffer, &vmaAllocationStaging, &stagingBufferAllocationInfo);

    void* mappedData = nullptr;
    vmaMapMemory(_context->allocator, vmaAllocationStaging, &mappedData);
    memcpy(mappedData, _data, _size);  // Copy data to the staging buffer

    vk::Buffer VertexBufferNuffer = VK_NULL_HANDLE;
    VmaAllocation vmaAllocationVertex = {};
    VmaAllocationInfo VertexBufferAllocationInfo = {};

    CreateBufferAndAlloc(_context, _size, static_cast<VkBufferUsageFlags>(GetVulkanUsage(usage)) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY,
        0,  // No flags for GPU-only memory
        &VertexBufferNuffer, &vmaAllocationVertex, &VertexBufferAllocationInfo);

    // Allocate and begin the command buffer
    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(_context->device, _commandPool);
    
    // Copy from staging to vertex buffer
    vk::BufferCopy copyRegion = {};
    copyRegion.size = _size;
    commandBuffer.copyBuffer(stagingNuffer, VertexBufferNuffer, 1, &copyRegion);

    EndSingleTimeCommands(commandBuffer);

    // Submit the command buffer and wait with a fence
    vk::SubmitInfo submitInfo = {};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vk::FenceCreateInfo fenceCreateInfo = {};
    vk::Fence fence = _context->device.createFence(fenceCreateInfo);

    VK_CALL(_context->vkQueues.graphicQueue.submit(1, &submitInfo, fence));
    VK_CALL(_context->device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX));

    _context->device.destroyFence(fence);
    
    // Cleanup staging buffer and unmap
    vmaUnmapMemory(_context->allocator, vmaAllocationStaging);
    vmaDestroyBuffer(_context->allocator, stagingNuffer, vmaAllocationStaging);
    
    *_vmaAllocation = vmaAllocationVertex;
    *_vkBuffer = VertexBufferNuffer;
}
