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

		VULKA_API void InstanceSwapBuffers() override;

		VULKA_API void InstanceBeginRender() override;
		
		VULKA_API void InstanceEndRender() override;

		VULKA_API void InstanceRender() override;

		VULKA_API void InstanceBindShaderProgram(const std::string& _shaderProgramName) override;

		
	private:
		VK_NP::VulkanMain m_VulkanMain;

		VULKA_API void InstanceCreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource) override;
		
		VULKA_API bool InstanceDestroyShader(const std::string& _shaderProgramName) override;
	};



}
