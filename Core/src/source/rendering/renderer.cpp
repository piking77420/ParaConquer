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
    InitShader();
}

void Renderer::Destroy()
{
    RHI::DestroyInstance();
}

void Renderer::Render()
{
    RHI::Render();
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


    RHI::BeginDraw();
    m_MainShader->Bind();
}

void Renderer::EndFrame()
{
    RHI::EndDraw();
}

void Renderer::SwapBuffers()
{
    RHI::SwapBuffers();
}

PC_CORE_API void Renderer::WaitDevice()
{
    RHI::WaitDevice();
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
