#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RhiDescriptorBindings.hpp"

namespace PC_CORE
{
    class Rhi;
}

namespace Vulkan
{
    class VulkanDescritptorManager;
    struct CacheDescriptor;
     

    class VulkanDescriptorBindings : public PC_CORE::RhiDescriptorBindings
    {
    public:
        VULKAN_API VulkanDescriptorBindings(PC_CORE::Rhi& _Rhi);

        VULKAN_API ~VulkanDescriptorBindings() override;

        VULKAN_API const void* GetFrameNativeHandle(size_t _FrameIndex) const override;

        VULKAN_API void* GetFrameNativeHandle(size_t _FrameIndex) override;

        VULKAN_API bool Build() override;

        VULKAN_API vk::DescriptorSet GetVkDescriptorSet(size_t _FrameIndex) const;

        VULKAN_API vk::DescriptorSet GetVkDescriptorSet() const;

    private:
        const CacheDescriptor& m_CacheDescriptor;

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
