#include "buffer/vulkan_uniform_buffer.hpp"

#include "utils/vulkan_buffer_helper.hpp"
#include "low_renderer/rhi.hpp"
#include "utils/vulkan_buffer_helper.hpp"

void Vulkan::VulkanUniformBuffer::MapData(void** _ptr)
{
    RhiUniformBuffer::MapData(_ptr);
    m_VulkanBuffer.MapData(_ptr);
}

void Vulkan::VulkanUniformBuffer::UnmapData()
{
    RhiUniformBuffer::UnmapData();
    m_VulkanBuffer.UnMapData();
}

Vulkan::VulkanUniformBuffer::VulkanUniformBuffer(const void* _data, uint32_t _sizeInByte, PC_CORE::MemoryLocalisation _visibility,
    PC_CORE::MemoryUsage _usage) : RhiUniformBuffer(_sizeInByte, _visibility, _usage),  m_VulkanBuffer(_data, _sizeInByte, vk::BufferUsageFlagBits::eUniformBuffer, _visibility, _usage)
{
    
}
