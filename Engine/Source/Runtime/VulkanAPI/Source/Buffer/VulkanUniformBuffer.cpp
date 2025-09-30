#include "Buffer/VulkanUniformBuffer.hpp"

#include "Utils/VulkanBufferHelper.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Utils/VulkanBufferHelper.hpp"

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
