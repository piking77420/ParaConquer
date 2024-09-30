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

VULKAN_API void VK_NP::VulkanApp::InstanceBindShaderProgram(const std::string& _shaderProgramName)
{
    m_VulkanMain.BindProgram(_shaderProgramName);
}


void VK_NP::VulkanApp::InstanceRecreateSwapChain(void* _glfwWindowptr, uint32_t _newWidht, uint32_t _newHeight)
{
    m_VulkanMain.RecreateSwapChain(_glfwWindowptr, _newWidht, _newHeight);
}


void VK_NP::VulkanApp::InstanceCreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    m_VulkanMain.CreateShader(programShaderCreateInfo, _shaderSource);
}

bool VK_NP::VulkanApp::InstanceDestroyShader(const std::string& _shaderProgramName)
{
    return m_VulkanMain.DestroyShader(_shaderProgramName);
}

VULKAN_API void VK_NP::VulkanApp::InstanceWaitDevice()
{
    m_VulkanMain.WaitDevice();
}

uint32_t VK_NP::VulkanApp::InstanceBufferData(size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage)
{
   return m_VulkanMain.bufferMap.CreateBuffer(static_cast<uint32_t>(_size), _data, _usage);
}

