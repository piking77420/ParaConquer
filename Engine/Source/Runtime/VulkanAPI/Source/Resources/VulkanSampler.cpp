#include "Resources/VulkanSampler.hpp"

#include "Utils/RhiToVulkan.hpp"
#include "VulkanDevice.hpp"
#include "LowRenderer/Rhi.hpp"

Vulkan::VulkanSampler::VulkanSampler(const PC_CORE::SamplerCreateInfo& _samplerCreateInfo) : RhiSampler(
    _samplerCreateInfo)
{
    const float maxAnisotopie = PC_CORE::Rhi::GetRhiContext()->physicalDevices->GetPhysicalDevice().
                                                               GetMaxSamplerAnisotropy();

    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.sType = vk::StructureType::eSamplerCreateInfo;
    samplerInfo.magFilter = Utils::RhiToVulkanFilter(magFilter);
    samplerInfo.minFilter = Utils::RhiToVulkanFilter(minFilter);
    samplerInfo.addressModeU = Utils::RhiToVulkanSamplerAddressMode(samU);
    samplerInfo.addressModeV = Utils::RhiToVulkanSamplerAddressMode(samV);
    samplerInfo.addressModeW = Utils::RhiToVulkanSamplerAddressMode(samW);
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
    // TODO NOT HARDCODED 
    samplerInfo.maxLod = static_cast<float>(16);
    samplerInfo.mipLodBias = 0.0f;

    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->
        GetDevice();

    m_Sampler = device.createSampler(samplerInfo);
}

Vulkan::VulkanSampler::~VulkanSampler()
{
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->
        GetDevice();
    device.destroySampler(m_Sampler);
}

vk::Sampler Vulkan::VulkanSampler::GetSampler() const
{
    return m_Sampler;
}
