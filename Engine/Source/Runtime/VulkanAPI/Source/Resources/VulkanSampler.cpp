#include "Resources/VulkanSampler.hpp"

#include "Utils/RhiToVulkan.hpp"
#include "VulkanDevice.hpp"
#include "LowRenderer/Rhi.hpp"

Vulkan::VulkanSampler::VulkanSampler(PC_CORE::Rhi& _Rhi, const std::string& _name, const PC_CORE::SamplerCreateInfo& _samplerCreateInfo)
    : RhiSampler(_Rhi, _name, _samplerCreateInfo)
{

}

Vulkan::VulkanSampler::~VulkanSampler()
{
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(m_Rhi.GetRhiContext().rhiDevice)->
        GetDevice();
    device.destroySampler(m_Sampler);
}


bool Vulkan::VulkanSampler::Build()
{
    const float maxAnisotopie = m_Rhi.GetRhiContext().rhiPhysicalDevices->GetPhysicalDevice().
                                                            GetMaxSamplerAnisotropy();

    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.sType = vk::StructureType::eSamplerCreateInfo;
    samplerInfo.magFilter = Utils::RhiToVulkanFilter(m_SamplerCreateInfo.magFilter);
    samplerInfo.minFilter = Utils::RhiToVulkanFilter(m_SamplerCreateInfo.minFilter);
    samplerInfo.addressModeU = Utils::RhiToVulkanSamplerAddressMode(m_SamplerCreateInfo.u);
    samplerInfo.addressModeV = Utils::RhiToVulkanSamplerAddressMode(m_SamplerCreateInfo.v);
    samplerInfo.addressModeW = Utils::RhiToVulkanSamplerAddressMode(m_SamplerCreateInfo.w);
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

    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(m_Rhi.GetRhiContext().rhiDevice)->
        GetDevice();

    m_Sampler = device.createSampler(samplerInfo);
    
    return true;
}


vk::Sampler Vulkan::VulkanSampler::GetSampler() const
{
    return m_Sampler;
}
