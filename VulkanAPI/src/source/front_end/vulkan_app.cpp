#include "front_end/vulkan_app.hpp"


VK_NP::VulkanApp::VulkanApp(const VulkanAppCreateInfo& vulkanMainCreateInfo) : m_VulkanMain(vulkanMainCreateInfo) 
{
}

VK_NP::VulkanApp::~VulkanApp()
{
}

void VK_NP::VulkanApp::InstanceSwapBuffers()
{
    m_VulkanMain.SwapBuffers();
}


void VK_NP::VulkanApp::InstanceBeginRender()
{
    m_VulkanMain.BeginDraw();
}

void VK_NP::VulkanApp::InstanceEndRender()
{
    m_VulkanMain.EnDraw();
}

void VK_NP::VulkanApp::InstanceRender()
{
    m_VulkanMain.Draw();
}

VULKA_API void VK_NP::VulkanApp::InstanceBindShaderProgram(const std::string& _shaderProgramName)
{
    m_VulkanMain.BindProgram(_shaderProgramName);
}


void VK_NP::VulkanApp::InstanceCreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    m_VulkanMain.CreateShader(programShaderCreateInfo, _shaderSource);
}

bool VK_NP::VulkanApp::InstanceDestroyShader(const std::string& _shaderProgramName)
{
    return m_VulkanMain.DestroyShader(_shaderProgramName);
}

