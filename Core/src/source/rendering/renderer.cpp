#include "rendering/renderer.hpp"

#include "io/window.hpp"
#include "world/transform.hpp"
#include "rendering/render_harware_interface/vertex.hpp"
#include "resources/resource_manager.hpp"
#include "resources/shader_source.hpp"

#undef ERROR


using namespace PC_CORE;


void Renderer::Init(GraphicAPI _graphicAPI, Window* _window)
{
    InitRhi(_graphicAPI, _window);
    m_RhiRef = RHI::GetInstance();
    InitCommandPools();
    InitShader();
    InitBuffer();


    const CommandBufferCreateInfo commandBufferCreateInfo =
        {
        .commandBufferPtr = m_SwapChainCommandBuffers.data(),
        .commandBufferCount = static_cast<uint32_t>(m_SwapChainCommandBuffers.size()),
        .commandBufferlevel = CommandBufferlevel::PRIMARY
        };
    m_SwapChainCommandPool.AllocCommandBuffer(commandBufferCreateInfo);
}

void Renderer::Destroy()
{
    m_SwapChainCommandPool.~CommandPool();
    m_TransfertPool.~CommandPool();

    RHI::DestroyInstance();
}

void Renderer::Render()
{
    Tbx::Vector2ui windowSize = Windowtpr->GetWindowSize();
    ViewPort viewport =
        {
        .position = {},
        .width = static_cast<float>(windowSize.x),
        .height = static_cast<float>(windowSize.y),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
        };

    ScissorRect ScissorRect;
    ScissorRect.offset = {},
    ScissorRect.extend = {
       windowSize.x,
       windowSize.y
    };
    
    m_RhiRef->SetScissor(m_CommandBuffer->handle, ScissorRect);
    
    m_RhiRef->SetViewPort(m_CommandBuffer->handle, viewport);
    
    m_MainShader->Bind(m_CommandBuffer->handle);
    Tbx::Vector3f color = {0,0,1};
    m_MainShader->PushVector3(m_CommandBuffer->handle,"PushConstants", &color);

    m_CommandBuffer->BindVertexBuffer(vertexBuffer, 0 ,1);
    m_CommandBuffer->BindIndexBuffer(indexBuffer);
    m_RhiRef->DrawIndexed(m_CommandBuffer->handle, indexBuffer.GetNbrOfIndicies(), 1, 0, 0, 0);
}

void Renderer::BeginFrame()
{
    static bool firstTime = false;
    if (glfwGetKey(Windowtpr->GetHandle(), GLFW_KEY_SPACE) == GLFW_PRESS && !firstTime)
    {
        m_RhiRef->WaitDevice();
        m_MainShader->Reload();
        firstTime = true;
    }

    if (glfwGetKey(Windowtpr->GetHandle(), GLFW_KEY_SPACE) == GLFW_RELEASE && firstTime)
    {
        firstTime = false;
    }

    m_RhiRef->WaitForAquireImage();
    m_CommandBuffer = &m_SwapChainCommandBuffers.at(static_cast<size_t>(m_RhiRef->GetCurrentImage()));

    
    m_RhiRef->BeginRender(m_CommandBuffer->handle);
}



void Renderer::SwapBuffers()
{
    m_RhiRef->EndRender();
    m_RhiRef->SwapBuffers(&m_CommandBuffer->handle, 1);
}

void Renderer::WaitDevice()
{
    m_RhiRef->WaitDevice();
}

void Renderer::RenderLog(LogType _logType, const char* _message)
{
    switch (_logType)
    {
    case LogType::INFO:
        PC_LOG(_message)
        break;
    case LogType::ERROR:
        PC_LOGERROR(_message)
        break;
    default: ;
    }
}

void Renderer::InitRhi(GraphicAPI _graphicAPI, Window* _window)
{
    Windowtpr = _window;

    switch (_graphicAPI)
    {
    case PC_CORE::GraphicAPI::NONE:
        break;
    case PC_CORE::GraphicAPI::VULKAN:
        {
            VK_NP::VulkanAppCreateInfo createInfo =
            {
                .appName = "Editor",
                .engineName = "ParaConquer Engine",
                .windowPtr = _window->GetHandle(),
                .logCallback = &Renderer::RenderLog,
            };
            RHI::MakeInstance(new VK_NP::VulkanApp(createInfo));
        }
        break;
    case PC_CORE::GraphicAPI::COUNT:
        break;
    case GraphicAPI::DX3D12:
        break;
    default:
        break;
    }
}

void Renderer::InitShader()
{
    ShaderSource* mainShaderVertex = ResourceManager::Get<ShaderSource>("main.vert");
    ShaderSource* mainShaderFrag = ResourceManager::Get<ShaderSource>("main.frag");

    PC_CORE::ProgramShaderCreateInfo createInfo{};
    createInfo.prograShaderName = "mainShader";

    createInfo.shaderInfo.shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS;
    std::get<ShaderGraphicPointInfo>(createInfo.shaderInfo.shaderInfoData).vertexInputBindingDescritions.push_back(
        Vertex::GetBindingDescrition(0));
    std::get<ShaderGraphicPointInfo>(createInfo.shaderInfo.shaderInfoData).vertexAttributeDescriptions = Vertex::GetAttributeDescriptions(0);
     

    m_MainShader = new ShaderProgram(createInfo, {mainShaderVertex, mainShaderFrag});
    ResourceManager::Add<ShaderProgram>(m_MainShader);
}

void Renderer::InitBuffer()
{
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.f}, {1.0f, 1.0f, 1.0f}, {1,0}},
        {{0.5f, -0.5f, 0.f}, {0.0f, 1.0f, 0.0f}, {0,1}},
        {{0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}, {1,1}},
            {{-0.5f, 0.5f,0.f}, {1.0f, 1.0f, 1.0f},{1,1}}
    };

    vertexBuffer = VertexBuffer(&m_TransfertPool ,vertices);

    const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };
    indexBuffer = IndexBuffer(&m_TransfertPool, indices);
}

void Renderer::InitCommandPools()
{
    CommandPoolCreateInfo commandPoolCreateInfo =
    {
        .queueType = QueuType::GRAPHICS,
        .commandPoolBufferFlag = COMMAND_POOL_BUFFER_RESET,
    };
    m_SwapChainCommandPool = CommandPool(commandPoolCreateInfo);

    commandPoolCreateInfo.queueType = QueuType::TRANSFERT;
    m_TransfertPool = CommandPool(commandPoolCreateInfo);
}
