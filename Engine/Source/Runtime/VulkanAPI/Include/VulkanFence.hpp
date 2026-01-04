#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RhiFence.hpp"

namespace Vulkan
{
    class VulkanFence final : public PC_CORE::RhiFence
    {
    public:
        VULKAN_API VulkanFence(PC_CORE::Rhi& _Rhi);

        ~VulkanFence() override;

        VULKAN_API bool Build() override;

        VULKAN_API void WaitForFence(bool _waitAll, uint32_t _time = UINT64_MAX) override;

        VULKAN_API void Reset() override;

        VULKAN_API vk::Fence GetVkFence(uint32_t _frameIndex) const;

    private:
        std::array<vk::Fence, MaxFramesInFlight> m_Fences
        {
            VK_NULL_HANDLE
        };
    };
}
