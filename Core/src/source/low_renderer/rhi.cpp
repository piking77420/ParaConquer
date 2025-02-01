#include "low_renderer/rhi.hpp"

#include "log.hpp"
#include "vulkan_command_list.hpp"
#include "vulkan_context.hpp"
#include "resources/resource_manager.hpp"
#include "resources/vulkan_sampler.hpp"
#include "resources/vulkan_shader_program.hpp"

using namespace PC_CORE;


Rhi::Rhi(Rhi&& other) noexcept
{

    m_RhiContext = other.m_RhiContext;
    other.m_RhiContext = nullptr;

    m_GraphicsApi = other.m_GraphicsApi;
    other.m_GraphicsApi = GraphicAPI::NONE;

    m_Instance = this;

}

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
    
    const SamplerCreateInfo createInfo =
          {
        .magFilter = Filter::LINEAR,
        .minFilter = Filter::LINEAR,
        .u = SamplerAddressMode::REPEAT,
        .v = SamplerAddressMode::REPEAT,
        .w = SamplerAddressMode::REPEAT
        };

    if (m_GraphicsApi == GraphicAPI::VULKAN)
    {
        m_RhiContext->sampler = std::make_unique<Vulkan::VulkanSampler>(createInfo);
    }
    else
    {
        PC_LOGERROR("DX12 NOT SUPPORTED YET")
    }
}

Rhi::~Rhi()
{
    if (m_Instance != nullptr && m_RhiContext != nullptr)
    {
      
        PC_LOG("Rhi Deinitialized");

        delete m_RhiContext;
        m_RhiContext = nullptr;

        m_Instance = nullptr;
    }
}

Rhi& Rhi::operator=(Rhi&& other) noexcept
{
    m_RhiContext = other.m_RhiContext;
    other.m_RhiContext = nullptr;

    m_GraphicsApi = other.m_GraphicsApi;
    other.m_GraphicsApi = GraphicAPI::NONE;

    m_Instance = this;


    return *this;
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

std::shared_ptr<CommandList> Rhi::CreateCommandList(const PC_CORE::CommandListCreateInfo& _commandListCreateInfo)
{
    Rhi& rhi = GetInstance();

    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanCommandList>(_commandListCreateInfo);
        break;
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::NONE:
    case GraphicAPI::COUNT:
        throw std::runtime_error("Invalid GraphicAPI");
    }

}

RhiContext* Rhi::GetRhiContext()
{
    return m_Instance->m_RhiContext;
}


void Rhi::NextFrame()
{
    m_Instance->m_CurrentFrame = (m_Instance->m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
   
}

uint32_t Rhi::GetFrameIndex() noexcept
{
    return (m_Instance) ? m_Instance->m_CurrentFrame : 0;
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
            SWAPCHAIN_EXT, //MESH_SHADER_EXT, ACCELERATION_EXT ,RAY_TRACING_EXT , DEFFERED_HOST_OP
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
  // m_RhiContext = std::make_shared<Vulkan::VulkanContext>(_createInfo);
    m_RhiContext = new Vulkan::VulkanContext(_createInfo);
}

void Rhi::DX12Initialize(const RhiContextCreateInfo& _createInfo)
{
}
