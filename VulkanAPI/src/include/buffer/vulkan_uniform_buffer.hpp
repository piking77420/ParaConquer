#pragma once

#include "vulkan_buffer.hpp"
#include "vulkan_header.h"
#include "low_renderer/rhi_uniform_buffer.hpp"

namespace Vulkan
{
    
    class VulkanUniformBuffer : public PC_CORE::RhiUniformBuffer
    {
    public:
        const void* GetNativeHandle() const override
        {
            return &m_VulkanBuffer;
        }

        void MapData(void** _ptr) override;
        
        void UnmapData() override;
        
        VulkanUniformBuffer(const void* _data, uint32_t _sizeInByte,
        PC_CORE::BufferMemoryUsage _usage);

        ~VulkanUniformBuffer() override = default;

   
    private:
        VulkanBuffer m_VulkanBuffer;

    };    
}

