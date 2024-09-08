#pragma once
#include "vulkan_header.h"


namespace VK_NP
{
	class VulkanApp
	{
	public:
		
		VULKA_API VulkanApp();

		VULKA_API ~VulkanApp();

	private:
		const char* m_AppName = nullptr;

		
	};



}
