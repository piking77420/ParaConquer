#pragma once

#include "VulkanBuffer.hpp"
#include "VulkanHeader.h"
#include "LowRenderer/RhiUniformBuffer.hpp"

namespace Vulkan
{
    
    class VulkanUniformBuffer : public PC_CORE::RhiUniformBuffer
    {
    public:
        const void* GetNativeHandle() const override
        {
            return &m_VulkanBuffer;
        }
        
        void* GetNativeHandle() override
        {
            return &m_VulkanBuffer;
        }
        
        void MapData(void** _ptr) override;
        
        void UnmapData() override;
        
        VulkanUniformBuffer(const void* _data, uint32_t _sizeInByte,PC_CORE::MemoryLocalisation _visibility,
        PC_CORE::MemoryUsage _usage);

        ~VulkanUniformBuffer() override = default;

   
    private:
        VulkanBuffer m_VulkanBuffer;

    };    
}

