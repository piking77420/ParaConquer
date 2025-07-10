#include "buffer/vulkan_index_buffer.hpp"

#include "utils/helper_functions.hpp"
#include "utils/vulkan_buffer_helper.hpp"

void Vulkan::VulkanIndexBuffer::MapData(void** _ptr)
{
    RhiIndexBuffer::MapData(_ptr);
    m_VulkanBuffer.MapData(_ptr);
}

void Vulkan::VulkanIndexBuffer::UnmapData()
{
    RhiIndexBuffer::UnmapData();
    m_VulkanBuffer.UnMapData();
}

Vulkan::VulkanIndexBuffer::VulkanIndexBuffer(const void* _data, uint32_t _sizeInByte, PC_CORE::IndexFormat _format, PC_CORE::MemoryLocalisation _visibility,
                                             PC_CORE::MemoryUsage _usage)  : m_VulkanBuffer(_data, _sizeInByte, vk::BufferUsageFlagBits::eIndexBuffer , _visibility, _usage)
{
    m_IndexFormat = _format;
    m_IndexCount = _sizeInByte / static_cast<int>(_format);
}

