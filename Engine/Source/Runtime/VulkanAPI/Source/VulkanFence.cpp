#include "VulkanFence.hpp"

#include "VulkanContext.hpp"
#include "LowRenderer/Rhi.hpp"


Vulkan::VulkanFence::VulkanFence(PC_CORE::Rhi& _Rhi)
    : RhiFence(_Rhi)
{
}

vk::Fence Vulkan::VulkanFence::GetVkFence(uint32_t _frameIndex) const
{
    return m_Fences[_frameIndex];
}

void Vulkan::VulkanFence::Reset()
{
    vk::Device d = GET_VK_DEVICE;
    const size_t frameIndex = m_Rhi.GetFrameIndex();

    vk::Fence f = GetVkFence(frameIndex);
    VK_CALL(d.resetFences(1, &f));
}

bool Vulkan::VulkanFence::Build()
{
    vk::Device d = GET_VK_DEVICE;

    for (auto& it : m_Fences)
    {
        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.sType = vk::StructureType::eFenceCreateInfo;
        fenceInfo.flags = GetSignaled()
            ? vk::FenceCreateFlagBits::eSignaled
            : static_cast<vk::FenceCreateFlagBits>(0);

        it = d.createFence(fenceInfo);
    }
    
    return true;
}

void Vulkan::VulkanFence::WaitForFence(bool _waitAll, uint32_t _time)
{
    vk::Device d = GET_VK_DEVICE;
    const size_t frameIndex = m_Rhi.GetFrameIndex();

    vk::Fence f = GetVkFence(frameIndex);
    VK_CALL(d.waitForFences(1, &f, vk::True, UINT64_MAX));
}

Vulkan::VulkanFence::~VulkanFence()
{
    vk::Device d = GET_VK_DEVICE;

    for (auto& it : m_Fences)
    {
        if (it != VK_NULL_HANDLE)
            d.destroyFence(it);
    }
}
