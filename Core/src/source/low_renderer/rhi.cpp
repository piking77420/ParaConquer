#include "low_renderer/rhi.hpp"

#include "log.hpp"
#include "vulkan_context.hpp"
#include "resources/resource_manager.hpp"
#include "resources/vulkan_shader_program.hpp"

using namespace PC_CORE;


Rhi::Rhi(const RenderHardwareInterfaceCreateInfo& _createInfo) : m_GraphicsApi(_createInfo.GraphicsAPI)
{
    PC_LOG("Rhi Initialize")
    if (m_Instance != nullptr)
    {
        PC_LOGERROR("Rhi already Existing");
        return;
    }
    m_Instance = this;
   
    Init(_createInfo);
}

Rhi::~Rhi()
{
    if (m_Instance != nullptr && m_RhiContext != nullptr)
    {
        PC_LOG("Rhi Deinitialized");
        m_Instance = nullptr;
    }
}

Rhi& Rhi::GetInstance()
{
    return *m_Instance;
}

ShaderProgram* Rhi::CreateShader(const ProgramShaderCreateInfo& _programShaderCreateInfo)
{
    Rhi& rhi = GetInstance();
    
    switch (rhi.m_GraphicsApi)
    {
        break;
    case GraphicAPI::VULKAN:
        return ResourceManager::Create<Vulkan::VulkanShaderProgram>(_programShaderCreateInfo);
        break;
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::NONE:
    case GraphicAPI::COUNT:
        break;
    }
    
}

void Rhi::Init(const RenderHardwareInterfaceCreateInfo& _createInfo)
{
    RenderInstanceCreateInfo renderInstanceCreateInfo =
           {
        .appName = _createInfo.appName
        };
    
    PhysicalDevicesCreateInfo physicalDevicesCreateInfo =
        {
        {
            SWAPCHAIN_EXT,MESH_SHADER_EXT, ACCELERATION_EXT ,RAY_TRACING_EXT , DEFFERED_HOST_OP
        }
        };

    const RhiContextCreateInfo renderContextCreateInfo =
        {
        _createInfo.window->GetHandle(),
        &renderInstanceCreateInfo,
        &physicalDevicesCreateInfo
        };

    
    switch (m_GraphicsApi)
    {
    case GraphicAPI::VULKAN:
        VulkanInitialize(renderContextCreateInfo);
        break;
    case GraphicAPI::DX3D12:
        DX12Initialize(renderContextCreateInfo);
        break;
    case GraphicAPI::COUNT:
    case GraphicAPI::NONE:
        break;
    }
}

void Rhi::VulkanInitialize(const RhiContextCreateInfo& _createInfo)
{
   m_RhiContext = std::make_shared<Vulkan::VulkanContext>(_createInfo);

}

void Rhi::DX12Initialize(const RhiContextCreateInfo& _createInfo)
{
    
}
