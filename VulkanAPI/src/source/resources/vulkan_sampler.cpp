#include "resources/vulkan_sampler.hpp"

#include "rhi_vulkan_parser.hpp"
#include "vulkan_device.hpp"
#include "low_renderer/rhi.hpp"

Vulkan::VulkanSampler::VulkanSampler(const PC_CORE::SamplerCreateInfo& _samplerCreateInfo) : RhiSampler(_samplerCreateInfo)
{
    const float maxAnisotopie = PC_CORE::Rhi::GetRhiContext()->physicalDevices->GetPhysicalDevice().GetMaxSamplerAnisotropy();
    
    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.sType = vk::StructureType::eSamplerCreateInfo;
    samplerInfo.magFilter = RHIToVulkanFilter(magFilter);
    samplerInfo.minFilter = RHIToVulkanFilter(minFilter);
    samplerInfo.addressModeU = RHIToVulkanSamplerAddressMode(samU);
    samplerInfo.addressModeV = RHIToVulkanSamplerAddressMode(samV);
    samplerInfo.addressModeW = RHIToVulkanSamplerAddressMode(samW);
    if (maxAnisotopie > 0.f)
    {
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = maxAnisotopie;
    }
    else
    {
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 0;
    }
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.minLod = 0.f;
    // TO DO NOT HARDCODED 
    samplerInfo.maxLod = static_cast<float>(16);
    samplerInfo.mipLodBias = 0.0f;

    vk::Device device =  std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->GetDevice();

    m_Sampler = device.createSampler(samplerInfo);
}

Vulkan::VulkanSampler::~VulkanSampler()
{
    vk::Device device =  std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->GetDevice();
    device.destroySampler(m_Sampler);
}

vk::Sampler Vulkan::VulkanSampler::GetSampler() const
{
    return m_Sampler;
}
