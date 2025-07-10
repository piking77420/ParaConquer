#include "buffer/vulkan_vertex_buffer.hpp"

#include "utils/helper_functions.hpp"
#include "utils/vulkan_buffer_helper.hpp"


void Vulkan::VulkanVertexBuffer::MapData(void** _ptr)
{
    RhiVertexBuffer::MapData(_ptr);
    m_VulkanBuffer.MapData(_ptr);
}

void Vulkan::VulkanVertexBuffer::UnmapData()
{
    RhiVertexBuffer::UnmapData();
    m_VulkanBuffer.UnMapData();
}

Vulkan::VulkanVertexBuffer::VulkanVertexBuffer(const void* _data, uint32_t _sizeInByte, PC_CORE::MemoryLocalisation _visibility, PC_CORE::MemoryUsage _usage) :
RhiVertexBuffer(_sizeInByte,_visibility,  _usage) , m_VulkanBuffer(_data, _sizeInByte, vk::BufferUsageFlagBits::eVertexBuffer, _visibility, _usage)
{
  
    
}

Vulkan::VulkanVertexBuffer::VulkanVertexBuffer(uint32_t _sizeInByte, PC_CORE::MemoryLocalisation _visibility,
    PC_CORE::MemoryUsage _usage) : 
RhiVertexBuffer(_sizeInByte,_visibility,  _usage) , m_VulkanBuffer(nullptr, _sizeInByte, vk::BufferUsageFlagBits::eVertexBuffer, _visibility, _usage)

{
    
}
