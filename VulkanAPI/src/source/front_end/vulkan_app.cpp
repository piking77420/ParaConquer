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


void VK_NP::VulkanApp::BeginRender()
{
    m_VulkanMain.BeginDraw();
}

void VK_NP::VulkanApp::EndRender()
{
    m_VulkanMain.EnDraw();
}

void VK_NP::VulkanApp::Render()
{
    m_VulkanMain.Draw();
}

VULKAN_API void VK_NP::VulkanApp::BindShaderProgram(const std::string& _shaderProgramName)
{
    m_VulkanMain.BindProgram(_shaderProgramName);
}


void VK_NP::VulkanApp::RecreateSwapChain(void* _glfwWindowptr, uint32_t _newWidht, uint32_t _newHeight)
{
    m_VulkanMain.RecreateSwapChain(_glfwWindowptr, _newWidht, _newHeight);
}

void VK_NP::VulkanApp::PushConstants(const std::string& _shaderProgramName,
            const std::string& _pushConstantName, const void* _data, uint32_t _size)
{
    m_VulkanMain.PushConstant(_shaderProgramName, _pushConstantName.c_str(), _data, _size);
}


void VK_NP::VulkanApp::CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    m_VulkanMain.CreateShader(programShaderCreateInfo, _shaderSource);
}

bool VK_NP::VulkanApp::DestroyShader(const std::string& _shaderProgramName)
{
    return m_VulkanMain.DestroyShader(_shaderProgramName);
}

VULKAN_API void VK_NP::VulkanApp::WaitDevice()
{
    m_VulkanMain.WaitDevice();
}

uint32_t VK_NP::VulkanApp::BufferData(size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage)
{
   return m_VulkanMain.bufferMap.CreateBuffer(static_cast<uint32_t>(_size), _data, _usage);
}

