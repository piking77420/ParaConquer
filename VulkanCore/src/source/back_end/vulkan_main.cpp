#include "pch.h"
#include "back_end/vulkan_main.h"

#include "front_end/vulkan_app.hpp"

VK_NP::VulkanMain::VulkanMain(const VulkanAppCreateInfo& vulkanMainCreateInfo) : m_vulkanHarwareWrapper(vulkanMainCreateInfo)
, m_vulkanPresentChain(vulkanMainCreateInfo)
{
    
}

void VK_NP::VulkanMain::SwapBuffers()
{
    
}

uint32_t VK_NP::VulkanMain::CreateShader(const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    return m_vulkanShaderManager.CreateShaderFromSource(_shaderSource);
}

bool VK_NP::VulkanMain::DestroyShader(uint32_t _shaderHandle)
{
    return m_vulkanShaderManager.DestroyShader(_shaderHandle);
}
