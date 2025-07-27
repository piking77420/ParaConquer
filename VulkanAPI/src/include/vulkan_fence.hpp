#pragma once

#include "vulkan_header.h"
#include "low_renderer/rhi_fence.hpp"

namespace Vulkan
{
	class VulkanFence final : public PC_CORE::RhiFence
	{
	public:

		VULKAN_API const void* GetNativeHandle() const override
		{
			return &m_Fences;
		}

		
		VULKAN_API void* GetNativeHandle() override
		{
			return &m_Fences;
		}

		VULKAN_API void WaitForFence(bool _waitAll, uint32_t _time = UINT64_MAX) override;

		VULKAN_API void Reset() override;

		VULKAN_API vk::Fence GetVkFence(uint32_t _frameIndex) const;

		VULKAN_API VulkanFence(const PC_CORE::RhiFenceCreateInfo& rhiFenceCreateInfo);

		VULKAN_API VulkanFence()
		{
			for (size_t i = 0; i < m_Fences.size(); i++)
				m_Fences[i] = VK_NULL_HANDLE;
			
		}

		~VulkanFence() override;

	private:

		std::array<vk::Fence, MAX_FRAMES_IN_FLIGHT> m_Fences;
	};

	
}