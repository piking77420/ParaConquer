#include "back_end/vulkan_shader_manager.hpp"

uint32_t VK_NP::VulkanShaderManager::CreateShaderFromSource(
    const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    //TODO PARSE SHADER

    for (uint32_t i = 0; i < _shaderSource.size(); i++)
    {
        
        
    }

}

bool VK_NP::VulkanShaderManager::DestroyShader(uint32_t shaderHandle)
{
    const bool success = m_InternalShadersMap.contains(shaderHandle);
    if (!success)
        return success;
    
    m_InternalShadersMap.erase(shaderHandle);
    return true;
}

VK_NP::VulkanShaderManager::~VulkanShaderManager()
{
    // TODO destoy all resour in backend shader internal
}
