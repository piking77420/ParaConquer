#pragma once

#include "VulkanBuffer.hpp"
#include "LowRenderer/RhiIndexBuffer.hpp"

namespace Vulkan
{
    class VulkanIndexBuffer : public PC_CORE::RhiIndexBuffer
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

        VulkanIndexBuffer(const void* _data, uint32_t _sizeInByte, PC_CORE::IndexFormat _format,
                          PC_CORE::MemoryLocalisation _visibility,
                          PC_CORE::MemoryUsage _usage);

        ~VulkanIndexBuffer() override = default;

    private:
        VulkanBuffer m_VulkanBuffer;
    };
}
