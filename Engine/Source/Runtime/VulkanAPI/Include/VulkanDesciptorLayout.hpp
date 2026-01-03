#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RhiDesciptorLayout.hpp"

namespace Vulkan
{
	class VULKAN_API VulkanDescriptorLayout : public PC_CORE::RHI::RhiDescriptorLayout
	{
	public:
		VulkanDescriptorLayout(PC_CORE::Rhi& _Rhi);

		~VulkanDescriptorLayout() override;

		virtual const void* GetFrameNativeHandle(size_t _frameIndex) const
		{
			return &m_DesciptorLayout;
		}


		virtual void* GetFrameNativeHandle(size_t _frameIndex)
		{
			return &m_DesciptorLayout;
		}

		vk::DescriptorSetLayout GetVkDesciptorSetLayout() const
		{
			return m_DesciptorLayout;
		}

		vk::DescriptorPool GetVkDesciptorPool() const
		{
			return m_DescriptorPool;
		}

	private:
		vk::DescriptorSetLayout m_DesciptorLayout = VK_NULL_HANDLE;

		vk::DescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

	};

}