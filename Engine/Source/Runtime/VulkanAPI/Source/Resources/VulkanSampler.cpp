#include "Resources/VulkanSampler.hpp"

#include "Utils/RhiToVulkan.hpp"
#include "VulkanDevice.hpp"
#include "LowRenderer/Rhi.hpp"

Vulkan::VulkanSampler::VulkanSampler(PC_CORE::Rhi& _Rhi)
    : RhiSampler(_Rhi)
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
    samplerInfo.magFilter = Utils::RhiToVulkanFilter(m_MagFilter);
    samplerInfo.minFilter = Utils::RhiToVulkanFilter(m_MinFilter);
    samplerInfo.addressModeU = Utils::RhiToVulkanSamplerAddressMode(m_U);
    samplerInfo.addressModeV = Utils::RhiToVulkanSamplerAddressMode(m_V);
    samplerInfo.addressModeW = Utils::RhiToVulkanSamplerAddressMode(m_W);
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
    samplerInfo.compareEnable = m_CompareEnable ? vk::True : vk::False;
    samplerInfo.compareOp = Utils::RhiToVulkanCompareOp(m_CompareOp);
    samplerInfo.mipmapMode = Utils::RhiToSamplerMipmapMode(m_MipmapMode);
    samplerInfo.minLod = 0.f;
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
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
