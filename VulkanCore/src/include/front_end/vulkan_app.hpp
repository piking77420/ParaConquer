#pragma once
#include "back_end/vulkan_main.h"

namespace VK_NP
{
	class VulkanApp
	{
	public:
		
		VULKA_API VulkanApp();

		VULKA_API ~VulkanApp();

	private:
		const char* m_AppName = nullptr;

		VK_NP::VulkanMain m_VulkanMain;
	};



}
