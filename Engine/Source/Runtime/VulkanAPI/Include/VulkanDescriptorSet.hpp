#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RhiDescriptorSet.hpp"

namespace PC_CORE
{
    class Rhi;
}

namespace Vulkan
{
    class VulkanDescriptorSet : public PC_CORE::RhiDescriptorSet
    {
    public:
        VULKAN_API VulkanDescriptorSet(PC_CORE::Rhi& _Rhi);

        VULKAN_API ~VulkanDescriptorSet() override;

        VULKAN_API bool Build() override;

        VULKAN_API vk::DescriptorSet GetVkDescriptorSet(size_t _FrameIndex) const;

        VULKAN_API vk::DescriptorSet GetVkDescriptorSet() const;

    private:
        std::array<vk::DescriptorSet, MaxFramesInFlight> m_DescriptorSets;

        void CreateDescriptors();

        void UpdateDesciptors();

        void FillDescriptorInfo(std::span<vk::DescriptorImageInfo>& imageInfo, std::span<vk::DescriptorBufferInfo>& bufferInfo, size_t& imageIndex, size_t& bufferIndex);

        void FillDescritptorWrite(std::span<vk::WriteDescriptorSet>& _WriteDescriptorSetSpan,
                                  const std::span<vk::DescriptorImageInfo>& imageInfo,
                                  const std::span<vk::DescriptorBufferInfo>& bufferInfo, 
                                  size_t& imageIndex,
                                  size_t& bufferIndex);
    };
}
