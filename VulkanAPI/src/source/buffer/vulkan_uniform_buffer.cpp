#include "buffer/vulkan_uniform_buffer.hpp"

#include "utils/vulkan_buffer_helper.hpp"
#include "low_renderer/rhi.hpp"
#include "utils/vulkan_buffer_helper.hpp"

void Vulkan::VulkanUniformBuffer::MapData(void** _ptr)
{
    auto& context = VulkanContext::GetContext();

    vk::Result r = static_cast<vk::Result>(vmaMapMemory(context.allocator,
                                                        m_VulkanBuffer.bufferAndAlloc[PC_CORE::Rhi::GetFrameIndex()].alloc, _ptr));
    VK_CALL(r);

    if (r != vk::Result::eSuccess)
        *_ptr = nullptr;
    
}

void Vulkan::VulkanUniformBuffer::UnmapData()
{
    auto& context = VulkanContext::GetContext();

    vmaUnmapMemory(context.allocator, m_VulkanBuffer.bufferAndAlloc[PC_CORE::Rhi::GetFrameIndex()].alloc);
}

Vulkan::VulkanUniformBuffer::VulkanUniformBuffer(const void* _data, uint32_t _sizeInByte,
                                                 PC_CORE::BufferMemoryUsage _usage)  :  m_VulkanBuffer(_usage)
{
    auto& context = VulkanContext::GetContext();
    
    for (size_t i = 0; i < m_VulkanBuffer.bufferAndAlloc.size(); i++)
    {
        Utils::CreateBuffer(context.allocator, _sizeInByte, vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU
                            , reinterpret_cast<VkBuffer*>(&m_VulkanBuffer.bufferAndAlloc[i].buffer), &m_VulkanBuffer.bufferAndAlloc[i].alloc);
    }
}
