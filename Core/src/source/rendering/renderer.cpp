﻿#include "rendering/renderer.hpp"

#include "io/window.hpp"
#include "world/transform.hpp"
#include "rendering/render_harware_interface/vertex.hpp"
#include "resources/resource_manager.hpp"
#include "resources/shader_source.hpp"

#undef ERROR
#undef near;
#undef far;


using namespace PC_CORE;


void Renderer::Init(GraphicAPI _graphicAPI, Window* _window)
{
    InitRhi(_graphicAPI, _window);
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

    for (auto&& uniform : m_SceneBufferUniforms)
        uniform.~UniformBuffer();

    RHI::DestroyInstance();
}

void Renderer::Render(const PC_CORE::RenderingContext& _renderingContext)
{
    UpdateUniforms(_renderingContext);
    
    ViewPort viewport =
    {
        .position = {},
        .width = static_cast<float>(_renderingContext.renderingContextSize.x),
        .height = static_cast<float>(_renderingContext.renderingContextSize.y),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    ScissorRect ScissorRect;
    ScissorRect.offset = {},
        ScissorRect.extend = {
            static_cast<uint32_t>(_renderingContext.renderingContextSize.x),
            static_cast<uint32_t>(_renderingContext.renderingContextSize.y)
        };


    RHI::GetInstance().SetScissor(m_CommandBuffer->handle, ScissorRect);
    RHI::GetInstance().SetViewPort(m_CommandBuffer->handle, viewport);


    m_MainShader->Bind(m_CommandBuffer->handle);

    Tbx::Matrix4x4f modelMatrix = Tbx::Matrix4x4f::Identity();
    m_MainShader->PushVector3(m_CommandBuffer->handle, "PushConstants", &modelMatrix);


    m_CommandBuffer->BindVertexBuffer(vertexBuffer, 0, 1);
    m_CommandBuffer->BindIndexBuffer(indexBuffer);
    RHI::GetInstance().DrawIndexed(m_CommandBuffer->handle, indexBuffer.GetNbrOfIndicies(), 1, 0, 0, 0);
}

void Renderer::BeginFrame()
{
    static bool firstTime = false;
    if (glfwGetKey(Windowtpr->GetHandle(), GLFW_KEY_SPACE) == GLFW_PRESS && !firstTime)
    {
        RHI::GetInstance().WaitDevice();
        m_MainShader->Reload();
        firstTime = true;
    }

    if (glfwGetKey(Windowtpr->GetHandle(), GLFW_KEY_SPACE) == GLFW_RELEASE && firstTime)
    {
        firstTime = false;
    }

    RHI::GetInstance().WaitForAquireImage();
    m_CommandBuffer = &m_SwapChainCommandBuffers.at(static_cast<size_t>(RHI::GetInstance().GetCurrentImage()));

    RHI::GetInstance().BeginRender(m_CommandBuffer->handle);
}


void Renderer::SwapBuffers()
{
    RHI::GetInstance().EndRender();
    RHI::GetInstance().SwapBuffers(&m_CommandBuffer->handle, 1);
}

void Renderer::WaitDevice()
{
    RHI::GetInstance().WaitDevice();
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
    std::get<ShaderGraphicPointInfo>(createInfo.shaderInfo.shaderInfoData).vertexAttributeDescriptions =
        Vertex::GetAttributeDescriptions(0);


    m_MainShader = new ShaderProgram(createInfo, {mainShaderVertex, mainShaderFrag});
    ResourceManager::Add<ShaderProgram>(m_MainShader);
}

void Renderer::InitBuffer()
{
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.f}, {1.0f, 1.0f, 1.0f}, {1, 0}},
        {{0.5f, -0.5f, 0.f}, {0.0f, 1.0f, 0.0f}, {0, 1}},
        {{0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}, {1, 1}},
        {{-0.5f, 0.5f, 0.f}, {1.0f, 1.0f, 1.0f}, {1, 1}}
    };

    vertexBuffer = VertexBuffer(&m_TransfertPool, vertices);

    const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };
    indexBuffer = IndexBuffer(&m_TransfertPool, indices);

    for (auto&& uniform : m_SceneBufferUniforms)
        uniform = UniformBuffer(&m_TransfertPool, sizeof(sceneBufferGPU));
}

void Renderer::UpdateUniforms(const RenderingContext& _renderingContext)
{
    const size_t currentFrame = static_cast<size_t>(RHI::GetInstance().GetCurrentImage());

    sceneBufferGPU.view = LookAtRH(_renderingContext.lowLevelCamera.position,
                                   _renderingContext.lowLevelCamera.position + _renderingContext.lowLevelCamera.front,
                                   _renderingContext.lowLevelCamera.up);
    sceneBufferGPU.proj = Tbx::PerspectiveMatrix(_renderingContext.lowLevelCamera.fov,
                                                 _renderingContext.lowLevelCamera.aspect,
                                                 _renderingContext.lowLevelCamera.near,
                                                 _renderingContext.lowLevelCamera.far);

    sceneBufferGPU.deltatime = _renderingContext.deltaTime;
    sceneBufferGPU.time = _renderingContext.time;
    m_SceneBufferUniforms[currentFrame].Update(sizeof(sceneBufferGPU), 0, &sceneBufferGPU);
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
