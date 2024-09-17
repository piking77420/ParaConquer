#include "pch.h"
#include "back_end/vulkan_main.h"

#include "front_end/vulkan_app.hpp"

VK_NP::VulkanMain::VulkanMain(const VulkanAppCreateInfo& vulkanMainCreateInfo) : m_vulkanHarwareWrapper(vulkanMainCreateInfo)
, m_vulkanPresentChain(vulkanMainCreateInfo)
{
    
}

VK_NP::VulkanMain::~VulkanMain()
{
}

void VK_NP::VulkanMain::SwapBuffers()
{
    
}

void VK_NP::VulkanMain::CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
   // m_vulkanShaderManager.CreateShaderFromSource(programShaderCreateInfo,_shaderSource);
}

bool VK_NP::VulkanMain::DestroyShader(const std::string&  _shaderName)
{
    //return m_vulkanShaderManager.DestroyShader(_shaderName);
    return false;
}
