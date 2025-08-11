#include "VulkanFence.hpp"

#include "VulkanContext.hpp"
#include "LowRenderer/Rhi.hpp"


vk::Fence Vulkan::VulkanFence::GetVkFence(uint32_t _frameIndex) const
{
	return m_Fences[_frameIndex];
}

void Vulkan::VulkanFence::Reset()
{
	vk::Device d = GET_VK_DEVICE->GetDevice();
	const size_t frameIndex = PC_CORE::Rhi::GetFrameIndex();

	vk::Fence f = GetVkFence(frameIndex);
	VK_CALL(d.resetFences(1, &f));
}

void Vulkan::VulkanFence::WaitForFence(bool _waitAll, uint32_t _time)
{
	vk::Device d = GET_VK_DEVICE->GetDevice();
	const size_t frameIndex = PC_CORE::Rhi::GetFrameIndex();

	vk::Fence f = GetVkFence(frameIndex);
	VK_CALL(d.waitForFences(1, &f, vk::True, UINT64_MAX));
}

Vulkan::VulkanFence::VulkanFence(const PC_CORE::RhiFenceCreateInfo& rhiFenceCreateInfo)
{
	vk::Device d = GET_VK_DEVICE->GetDevice();

	for (auto& it : m_Fences)
	{
		vk::FenceCreateInfo fenceInfo{};
		fenceInfo.sType = vk::StructureType::eFenceCreateInfo;
		fenceInfo.flags = rhiFenceCreateInfo.signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlagBits(0);

		it = d.createFence(fenceInfo);
	}
}

Vulkan::VulkanFence::~VulkanFence()
{
	vk::Device d = GET_VK_DEVICE->GetDevice();

	for (auto& it : m_Fences)
	{
		d.destroyFence(it);
	}
}
