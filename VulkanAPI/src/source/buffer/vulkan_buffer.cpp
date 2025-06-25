#include "buffer/vulkan_buffer.hpp"

#include "vulkan_buffer_helper.hpp"


Vulkan::VulkanBuffer::VulkanBuffer(PC_CORE::BufferMemoryUsage _usage)
{
    bufferAndAlloc.resize(MAX_FRAMES_IN_FLIGHT);
}

Vulkan::VulkanBuffer::~VulkanBuffer()
{
    for (auto& alloc : bufferAndAlloc)
    {
        if (alloc.buffer == VK_NULL_HANDLE || alloc.alloc == VK_NULL_HANDLE)
            return;
        auto& context = VulkanContext::GetContext();

        DestroyBuffer(context.GetDevice()->GetDevice(), context.allocator, alloc.buffer,alloc.alloc);
        alloc.buffer = VK_NULL_HANDLE;
        alloc.alloc = VK_NULL_HANDLE;
    }
}
