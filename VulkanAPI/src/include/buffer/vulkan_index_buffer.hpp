#pragma once

#include "vulkan_buffer.hpp"
#include "low_renderer/rhi_index_buffer.hpp"

namespace Vulkan
{

class VulkanIndexBuffer : public PC_CORE::RhiIndexBuffer
{
public:
    const void* GetNativeHandle() const override
    {
        return &m_VulkanBuffer;
    }

    VulkanIndexBuffer(const void* _data, uint32_t _sizeInByte, PC_CORE::IndexFormat _format,
                      PC_CORE::BufferMemoryUsage _usage);

    ~VulkanIndexBuffer() override = default;
    
private:
    VulkanBuffer m_VulkanBuffer;

   
};
    
}
