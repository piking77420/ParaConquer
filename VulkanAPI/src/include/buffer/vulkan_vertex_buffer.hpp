#pragma once

#include "vulkan_buffer.hpp"
#include "low_renderer/rhi_vertex_buffer.hpp"

namespace Vulkan
{
    class VulkanVertexBuffer : public PC_CORE::RhiVertexBuffer
    {
    public:

        const void* GetNativeHandle() const override 
        {
            return &m_VulkanBuffer;
        }

        void MapData(void** _ptr) override;
        
        void UnmapData() override;
        
        DEFAULT_COPY_MOVE_OPERATIONS(VulkanVertexBuffer)
        
        VULKAN_API VulkanVertexBuffer(const void* _data, uint32_t _sizeInByte, PC_CORE::MemoryLocalisation _visibility, PC_CORE::MemoryUsage _usage);

        VULKAN_API VulkanVertexBuffer() = default;
        
        VULKAN_API ~VulkanVertexBuffer() override = default;
    private:
        VulkanBuffer m_VulkanBuffer; 
    };    
}

