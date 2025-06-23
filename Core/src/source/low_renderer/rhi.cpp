#include "low_renderer/rhi.hpp"

#include "log.hpp"
#include "vulkan_command_list.hpp"
#include "vulkan_context.hpp"
#include "vulkan_frame_buffer.hpp"
#include "vulkan_render_pass.hpp"
#include "handles/vulkan_buffer_handle.hpp"
#include "handles/vulkan_image_handle.hpp"
#include "resources/vulkan_descriptor_sets.hpp"
#include "resources/vulkan_sampler.hpp"
#include "resources/vulkan_shader_program.hpp"

using namespace PC_CORE;


Rhi::Rhi(Rhi&& other) noexcept
{
    m_RhiContext = other.m_RhiContext;
    other.m_RhiContext = nullptr;

    m_GraphicsApi = other.m_GraphicsApi;
    other.m_GraphicsApi = GraphicAPI::NONE;

    std::exchange(m_GpuResource, other.m_GpuResource);
    
    std::exchange(m_GPUHandleIdStack, other.m_GPUHandleIdStack);

    m_Instance = this;

}

Rhi& Rhi::operator=(Rhi&& other) noexcept
{
    m_RhiContext = other.m_RhiContext;
    other.m_RhiContext = nullptr;

    m_GraphicsApi = other.m_GraphicsApi;
    other.m_GraphicsApi = GraphicAPI::NONE;


    std::exchange(m_GpuResource, other.m_GpuResource);

    std::exchange(m_GPUHandleIdStack, other.m_GPUHandleIdStack);

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

    // init id stack
    for (GPUHandleID i = 0; i < MAX_ID; i++)
    {
        m_GPUHandleIdStack.push(i);
    }
    
}

Rhi::~Rhi()
{
    if (m_Instance != nullptr && m_RhiContext != nullptr)
    {
        PC_LOG("Rhi Deinitialized");

        for (auto& it : m_GpuResource)
        {
           
            
            if (it.second.use_count() != 1)
            {
                PC_LOGERROR("While free all gpuresource There is still a pointer pointing to resource");
            }

            it.second.reset();
        }

        delete m_RhiContext;
        m_RhiContext = nullptr;

        m_Instance = nullptr;
    }
}

Rhi& Rhi::GetInstance()
{
    return *m_Instance;
}

std::shared_ptr<ShaderProgram> Rhi::CreateShader(const ProgramShaderCreateInfo& _programShaderCreateInfo)
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

void Rhi::DestroyShader(ShaderProgram* _shaderprogram)
{
    delete _shaderprogram;
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
    default: ;
    }
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
    default: ;
    }
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
    default:;
    }
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
    default: ;
    }
}

GPUHandleID Rhi::CreateBuffer(const GPUBufferCreateInfo& _bufferCreateInfo)
{
    std::shared_ptr<GPUResource> bufferPtr = std::make_shared<Vulkan::VulkanBufferHandle>(_bufferCreateInfo);
    

    GPUHandleID id = CreateGpuHandle();
    m_Instance->m_GpuResource.insert({id,bufferPtr});

    return id;
}

bool Rhi::DestroyGpuHandle(GPUHandleID _gpuHandleID)
{
    if (_gpuHandleID == GPU_INVALID_ID)
    {
        PC_LOGERROR("Invalid GPUHandleID");
        return false;
    }
    
    auto it = m_Instance->m_GpuResource.find(_gpuHandleID);
    
    if (it == m_Instance->m_GpuResource.end())
    {
        PC_LOGERROR("Gpu handle match no gpu resource while try destroy");
        return false;
    }
    
    if (it->second.use_count() != 1)
    {
        PC_LOGERROR("Delect a gpu resourece while multiple pointer pointing on it");
        it->second.reset();
    }
    else
    {
        it->second.reset();
    }
  
    m_Instance->m_GPUHandleIdStack.push(_gpuHandleID);
    m_Instance->m_GpuResource.erase(_gpuHandleID);

    return true;
}

void Rhi::MapBuffer(GPUHandleID _gPUHandleID, void** _ptr)
{
    std::shared_ptr<GPUResource> resource = GetGpuResource(_gPUHandleID);
    
    if (resource == nullptr)
        return;

    switch (m_Instance->m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        std::reinterpret_pointer_cast<Vulkan::VulkanBufferHandle>(resource)->MapBuffer(_ptr);
        break;
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    }
    
}

void Rhi::UnMapBuffer(GPUHandleID _gPUHandleID)
{
    std::shared_ptr<GPUResource> resource = GetGpuResource(_gPUHandleID);
    
    if (resource == nullptr)
        return;

    switch (m_Instance->m_GraphicsApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        std::reinterpret_pointer_cast<Vulkan::VulkanBufferHandle>(resource)->UnMapBuffer();
        break;
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
        break;
    }
    
  
}

GPUHandleID Rhi::CreateImage(const CreateImageInfo& _createImage)
{
    
    std::shared_ptr<GPUResource> bufferPtr = std::make_shared<Vulkan::VulkanImageHandle>(_createImage);
  
    GPUHandleID id = CreateGpuHandle();
    m_Instance->m_GpuResource.insert({id,bufferPtr});

    return id;
}

std::shared_ptr<GPUResource> Rhi::GetResourceFromHandle(GPUHandleID _gpuHandleId)
{
    auto it = m_Instance->m_GpuResource.find(_gpuHandleId);
    return it != m_Instance->m_GpuResource.end() ? it->second : nullptr;
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
    m_RhiContext = new Vulkan::VulkanContext(_createInfo);
}

void Rhi::DX12Initialize(const RhiContextCreateInfo& _createInfo)
{
}

GPUHandleID Rhi::CreateGpuHandle()
{
    if (m_Instance->m_GPUHandleIdStack.empty())
    {
        PC_LOGERROR("Max GPUHandle has been reach");
        return GPU_INVALID_ID;
    }
    
    GPUHandleID outHandleId = m_Instance->m_GPUHandleIdStack.top();
    m_Instance->m_GPUHandleIdStack.pop();
    return outHandleId;
}

std::shared_ptr<GPUResource> Rhi::GetGpuResource(GPUHandleID _gpuHandleId)
{
    assert(_gpuHandleId != GPU_INVALID_ID && "Invalid GPUHandleID");

    auto it = m_Instance->m_GpuResource.find(_gpuHandleId);
    if (it == m_Instance->m_GpuResource.end())
    {
        PC_LOGERROR("Cannot map GPU buffer");
        return nullptr;
    }

    return it->second;
}
