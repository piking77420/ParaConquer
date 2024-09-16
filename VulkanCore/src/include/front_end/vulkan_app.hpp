#pragma once

#include "rendering/RHI.hpp"

#include "back_end/vulkan_main.h"

namespace VK_NP
{
	class VulkanApp : public PC_CORE::RHI
	{
	public:
		
		VULKA_API VulkanApp(const VulkanAppCreateInfo& vulkanMainCreateInfo);

		VULKA_API ~VulkanApp() override;

		VULKA_API void SwapBuffers() override;
		
		VULKA_API bool InstanceCompileShadersSource(const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource) override;
		
		VULKA_API bool InstanceDestroyShader(uint32_t _shaderHandle) override;
		
	private:
		VK_NP::VulkanMain m_VulkanMain;
	};



}
