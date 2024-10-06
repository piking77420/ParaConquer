#include "rendering/renderer.hpp"
#include "rendering/renderer.hpp"

#include "io/window.hpp"
#include "world/transform.hpp"
#include "io/window.hpp"
#include "rendering/render_harware_interface/gpu_buffer.h"
#include "rendering/render_harware_interface/vertex.hpp"
#include "resources/resource_manager.hpp"
#include "resources/shader_source.hpp"

using namespace PC_CORE;


void Renderer::Init(GraphicAPI _graphicAPI, Window* _window)
{
    InitRhi(_graphicAPI, _window);
    m_RhiRef = RHI::GetInstance();
    InitShader();
    InitBuffer();

    CommandPoolCreateInfo commandPoolCreateInfo =
        {
            .queuTypeUsage = QueuType::GRAPHICS
        };
    m_SwapChainCommandPool = CommandPool(commandPoolCreateInfo);
    m_SwapChainCommandPool.AllocCommandBuffers(m_SwapChainCommandBuffers.data(),m_SwapChainCommandBuffers.size());
}

void Renderer::Destroy()
{
    m_SwapChainCommandPool.FreeCommandBuffers(m_SwapChainCommandBuffers.data(), m_SwapChainCommandBuffers.size());
    vertexBuffer.~GpuBuffer();
    
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
    
    m_RhiRef->SetScissor(*m_CommandBuffer, ScissorRect);
    
    m_RhiRef->SetViewPort(*m_CommandBuffer, viewport);
    
    m_MainShader->Bind(*m_CommandBuffer);
    //Tbx::Vector3f color = {0,0,1};
    //m_MainShader->PushVector3(*m_CommandBuffer,"PushConstants", &color);

    vertexBuffer.Bind(*m_CommandBuffer);
    m_RhiRef->Draw(*m_CommandBuffer,3, 1, 0, 0);
}

void Renderer::BeginFrame()
{
    static bool firstTime = false;
    if (glfwGetKey(Windowtpr->GetHandle(), GLFW_KEY_SPACE) == GLFW_PRESS && !firstTime)
    {
        m_MainShader->Reload();
        firstTime = true;
    }

    if (glfwGetKey(Windowtpr->GetHandle(), GLFW_KEY_SPACE) == GLFW_RELEASE && firstTime)
    {
        firstTime = false;
    }

    m_RhiRef->WaitForAquireImage();
    m_CommandBuffer = &m_SwapChainCommandBuffers.at(static_cast<size_t>(m_RhiRef->GetCurrentImage()));

    
    m_RhiRef->BeginRender(*m_CommandBuffer);
}



void Renderer::SwapBuffers()
{
    m_RhiRef->EndRender();
    m_RhiRef->SwapBuffers(m_CommandBuffer, 1);
}

void Renderer::WaitDevice()
{
    m_RhiRef->WaitDevice();
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
                .windowPtr = _window->GetHandle()
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
        {{0.0f, -0.5f, 0.f}, {1.0f, 1.0f, 1.0f}, {1,0}},
        {{0.5f, 0.5f, 0.f}, {0.0f, 1.0f, 0.0f}, {0,1}},
        {{-0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}, {1,1}}
    };

    vertexBuffer = VertexBuffer(sizeof(vertices[0]) * vertices.size(), vertices[0].position.GetPtr());
}
