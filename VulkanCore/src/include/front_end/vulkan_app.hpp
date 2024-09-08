#pragma once
#include "vulkan_header.h"


namespace VK_NP
{
	class VulkanApp
	{
	public:
		
		VULKA_API VulkanApp() = delete;

		VULKA_API ~VulkanApp() = delete;

	private:
		const char* m_AppName = nullptr;

		
	};



}
