#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/DescriptorSet.hpp"

namespace PC_CORE
{
    class Rhi;
}

namespace Vulkan
{
    class VulkanDescritptorManager;
    struct CacheDescriptor;
     

    class VulkanDescriptorSets : public PC_CORE::ShaderProgramDescriptorSets
    {
    public:

        VULKAN_API VulkanDescriptorSets(PC_CORE::Rhi& _Rhi, const CacheDescriptor& _Cache);

        VULKAN_API ~VulkanDescriptorSets() override;

        VULKAN_API const void* GetFrameNativeHandle(size_t _frameIndex) const override;

        VULKAN_API void* GetFrameNativeHandle(size_t _frameIndex) override;

        VULKAN_API bool Build() override;


    private:
        const CacheDescriptor& m_Cache;

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
