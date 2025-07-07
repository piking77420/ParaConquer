#include "low_renderer/rhi.hpp"

#include "log.hpp"
#include "vulkan_command_list.hpp"
#include "vulkan_context.hpp"
#include "vulkan_frame_buffer.hpp"
#include "vulkan_render_pass.hpp"
#include "buffer/vulkan_index_buffer.hpp"
#include "buffer/vulkan_uniform_buffer.hpp"
#include "buffer/vulkan_vertex_buffer.hpp"
#include "resources/vulkan_descriptor_sets.hpp"
#include "resources/vulkan_sampler.hpp"
#include "resources/vulkan_shader_program.hpp"
#include "texture/vulkan_texture_2d.hpp"
#include "texture/vulkan_texture_3d.hpp"

using namespace PC_CORE;


Rhi::Rhi(Rhi&& other) noexcept
{
    m_RhiContext = other.m_RhiContext;
    other.m_RhiContext = nullptr;

    m_GraphicsApi = other.m_GraphicsApi;
    other.m_GraphicsApi = GraphicAPI::NONE;

    m_Instance = this;

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


Rhi::Rhi(const RenderHardwareInterfaceCreateInfo& _createInfo) : m_GraphicsApi(_createInfo.GraphicsAPI)
{
    PERF_REGION_SCOPED;
    
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
        
        delete m_RhiContext;
        m_RhiContext = nullptr;

        m_Instance = nullptr;
    }
}

Rhi& Rhi::GetInstance()
{
    return *m_Instance;
}

std::shared_ptr<RhiShaderProgram> Rhi::CreateRhiShaderProgram(const ProgramShaderCreateInfo& _programShaderCreateInfo)
{
    Rhi& rhi = GetInstance();
    
    switch (rhi.m_GraphicsApi)
    {
        break;
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanShaderProgram>(_programShaderCreateInfo);
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

std::shared_ptr<RhiRenderPass> Rhi::CreateRenderPass(PC_CORE::RHIFormat _colorFormat, PC_CORE::RHIFormat _depthFormat)
{
    Rhi& rhi = GetInstance();


    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanRenderPass>(_colorFormat, _depthFormat);
        break;
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    default:
    assert(false);
    }

    return nullptr;
}

std::shared_ptr<RhiRenderPass> Rhi::CreateRenderPass(PC_CORE::RHIFormat _colorFormat)
{
    Rhi& rhi = GetInstance();


    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanRenderPass>(_colorFormat);
        break;
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    default: 
        assert(false);
    }
    return nullptr;
}

PC_CORE_API std::shared_ptr<RhiRenderPass> Rhi::CreateRenderPass(PC_CORE::RHIFormat _colorFormat, uint32_t sampleCount)
{
    Rhi& rhi = GetInstance();


    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanRenderPass>(_colorFormat, sampleCount);
        break;
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    default:
        assert(false);
    }
    return nullptr;
}

std::shared_ptr<RhiRenderPass> Rhi::CreateRenderPass(const RenderPassDescriptor& _renderPassDescriptor)
{
    Rhi& rhi = GetInstance();


    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanRenderPass>(_renderPassDescriptor);
        break;
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    default: 
        assert(false);
    }

    return nullptr;
}

std::shared_ptr<FrameBuffer> Rhi::CreateFrameBuffer(const CreateFrameInfo& _createFrameInfo)
{
    Rhi& rhi = GetInstance();


    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanFrameBuffer>(_createFrameInfo);
        break;
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    default: assert(false);
    }
}

std::shared_ptr<RhiIndexBuffer> Rhi::CreateIndexBuffer(const void* _data, uint32_t _sizeInByte, IndexFormat _format, MemoryLocalisation _visibility,
    MemoryUsage _usage)
{
    Rhi& rhi = GetInstance();

    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanIndexBuffer>(_data, _sizeInByte, _format,_visibility, _usage);
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::NONE:
    case GraphicAPI::COUNT:
        break;
    default: assert(false);
    }

    return nullptr;
}

std::shared_ptr<RhiVertexBuffer> Rhi::CreateVertexBuffer(const void* _data, uint32_t _sizeInByte, MemoryLocalisation _visibility, MemoryUsage _usage)
{
    Rhi& rhi = GetInstance();

    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanVertexBuffer>(_data, _sizeInByte,_visibility, _usage);
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::NONE:
    case GraphicAPI::COUNT:
    default: assert(false);
    }
    return nullptr;
}

std::shared_ptr<RhiVertexBuffer> Rhi::CreateVertexBuffer(uint32_t _sizeInByte, MemoryLocalisation _visibility,
    MemoryUsage _usage)
{
    Rhi& rhi = GetInstance();

    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanVertexBuffer>(_sizeInByte, _visibility, _usage);
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    default: 
        assert(false);
    }
    return nullptr;
}

std::shared_ptr<RhiUniformBuffer> Rhi::CreateUniformBuffer(const void* _data, uint32_t _sizeInByte, MemoryLocalisation _visibility, MemoryUsage _usage)
{
    Rhi& rhi = GetInstance();

    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanUniformBuffer>(_data, _sizeInByte, _visibility, _usage);
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    default: 
        assert(false);
    }
}


std::shared_ptr<RhiTexture2D> Rhi::CreateTexture2D(const PC_CORE::CreateImageInfo2D& _createImageInfo)
{
    Rhi& rhi = GetInstance();

    static_assert(std::is_base_of_v<RhiTexture2D, Vulkan::VulkanTexture2D>,"");
    
    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanTexture2D>(_createImageInfo);
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    default: 
        assert(false);
    }
}

std::shared_ptr<RhiTexture3D> Rhi::CreateTexture3D(const PC_CORE::CreateImageInfo3D& _createImageInfo3D)
{
    Rhi& rhi = GetInstance();

    static_assert(std::is_base_of_v<RhiTexture3D, Vulkan::VulkanTexture3D>,"");
    
    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanTexture3D>(_createImageInfo3D);
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    default: ;
    }
}

std::shared_ptr<RhiSampler> Rhi::CreateSampler(const PC_CORE::SamplerCreateInfo& _samplerCreateInfo)
{
    Rhi& rhi = GetInstance();

    static_assert(std::is_base_of_v<RhiSampler, Vulkan::VulkanSampler>,"");
    
    switch (rhi.m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        return std::make_shared<Vulkan::VulkanSampler>(_samplerCreateInfo);
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    default: ;
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

void Rhi::Init(const RenderHardwareInterfaceCreateInfo& _createInfo)
{
    RenderInstanceCreateInfo renderInstanceCreateInfo =
        {
        .appName = _createInfo.appName,
        .gpuDebug = _createInfo.gpuDebug
        };
    
    PhysicalDevicesCreateInfo physicalDevicesCreateInfo =
        {
            {
                SWAPCHAIN_EXT, //MESH_SHADER_EXT, ACCELERATION_EXT ,RAY_TRACING_EXT , DEFFERED_HOST_OP
            },
        
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
    m_RhiContext = new Vulkan::VulkanContext(_createInfo);
}

void Rhi::DX12Initialize(const RhiContextCreateInfo& _createInfo)
{
}
