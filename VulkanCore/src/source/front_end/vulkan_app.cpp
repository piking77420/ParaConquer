#include "front_end/vulkan_app.hpp"



VK_NP::VulkanApp::VulkanApp(const VulkanAppCreateInfo& vulkanMainCreateInfo) : m_VulkanMain(vulkanMainCreateInfo) 
{
    
}

VK_NP::VulkanApp::~VulkanApp()
{
}

void VK_NP::VulkanApp::SwapBuffers()
{
    m_VulkanMain.SwapBuffers();
}

bool VK_NP::VulkanApp::InstanceCompileShadersSource(const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    return m_VulkanMain.CreateShader(_shaderSource);
}

bool VK_NP::VulkanApp::InstanceDestroyShader(uint32_t _shaderHandle)
{
    return m_VulkanMain.DestroyShader(_shaderHandle);
}

