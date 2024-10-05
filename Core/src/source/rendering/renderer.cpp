#include "rendering/renderer.hpp"
#include "rendering/renderer.hpp"

#include "io/window.hpp"
#include "world/transform.hpp"
#include "io/window.hpp"
#include "rendering/gpu_buffer.h"
#include "rendering/vertex.hpp"
#include "resources/resource_manager.hpp"
#include "resources/shader_source.hpp"

using namespace PC_CORE;

const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f, 0.f}, {1.0f, 1.0f, 1.0f}, {}},
    {{0.5f, 0.5f, 0.f}, {0.0f, 1.0f, 0.0f}, {}},
    {{-0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}, {}}
};


void Renderer::Init(GraphicAPI _graphicAPI, Window* _window)
{
    InitRhi(_graphicAPI, _window);
    m_RhiRef = RHI::GetInstance();
    InitShader();
}

void Renderer::Destroy()
{
    RHI::DestroyInstance();
}

void Renderer::Render()
{
    m_RhiRef->Render();
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


    m_RhiRef->BeginRender();
    m_MainShader->Bind();

    Tbx::Vector3f color = {1,0,1};
    m_MainShader->PushVector3("PushConstants", &color);
}

void Renderer::EndFrame()
{
    m_RhiRef->EndRender();
}

void Renderer::SwapBuffers()
{
    m_RhiRef->SwapBuffers();
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
    std::get<ShaderGraphicPointInfo>(createInfo.shaderInfo.shaderInfoData).vertexInputBindingDescrition.push_back(
        Vertex::GetBindingDescrition(0));

    m_MainShader = new ShaderProgram(createInfo, {mainShaderVertex, mainShaderFrag});
    ResourceManager::Add<ShaderProgram>(m_MainShader);
}

void Renderer::InitBuffer()
{
    return;
    VertexBuffer vertexBuffer;
    vertexBuffer.Alloc(sizeof(vertices[0]) * vertices.size(), vertices[0].position.GetPtr());
}
