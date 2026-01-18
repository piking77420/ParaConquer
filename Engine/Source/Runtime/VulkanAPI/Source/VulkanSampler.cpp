#include "VulkanSampler.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Utils/RhiToVulkan.hpp"
#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"

Vulkan::VulkanSampler::VulkanSampler(PC_CORE::Rhi& _Rhi)
    : RhiSampler(_Rhi)
{

}

Vulkan::VulkanSampler::~VulkanSampler()
{
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(m_Rhi.GetRhiContext().rhiDevice)->
        GetDevice();


    if (m_Sampler != VK_NULL_HANDLE)
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
        samplerInfo.maxAnisotropy = 1;
    }
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.compareEnable = m_CompareEnable ? vk::True : vk::False;
    samplerInfo.compareOp = Utils::RhiToVulkanCompareOp(m_CompareOp);
    samplerInfo.mipmapMode = Utils::RhiToSamplerMipmapMode(m_MipmapMode);
    samplerInfo.minLod = 0.f;
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;


    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(m_Rhi.GetRhiContext().rhiDevice)->
        GetDevice();

    m_Sampler = device.createSampler(samplerInfo);

    vk::DebugUtilsObjectNameInfoEXT nameInfoImageView;
    nameInfoImageView.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
    nameInfoImageView.pNext = nullptr;
    nameInfoImageView.objectType = vk::ObjectType::eSampler;
    nameInfoImageView.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkSampler>(m_Sampler));
    nameInfoImageView.pObjectName = GetName().data();

    SET_VK_DEBUG_NAME(nameInfoImageView);
    
    return true;
}


vk::Sampler Vulkan::VulkanSampler::GetVkSampler() const
{
    return m_Sampler;
}
