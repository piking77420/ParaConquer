#pragma once

#include "vulkan_harware_wrapper.hpp"
#include "vulkan_header.h"

namespace VK_NP
{
	class VulkanMain
	{
	public:
		VULKA_API VulkanMain() = default;

		VULKA_API ~VulkanMain() = default;

	private:
		VulkanHarwareWrapper m_vulkanHarwareWrapper;
	};
}
