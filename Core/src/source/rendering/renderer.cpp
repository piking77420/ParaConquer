﻿#include "rendering/renderer.hpp"
#include "rendering/renderer.hpp"

#include "io/window.hpp"
#include "world/transform.hpp"
#include "io/window.hpp"
#include "resources/resource_manager.hpp"
#include "resources/shader_source.hpp"

using namespace PC_CORE;


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
    RHI::BeginDraw();
    RHI::BindShaderProgram(mainShader);
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
    switch (_graphicAPI)
    {
    case PC_CORE::GraphicAPI::NONE:
        break;
    case PC_CORE::GraphicAPI::VULKAN:
        {
            VulkanAppCreateInfo createInfo =
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
    default:
        break;
    }
}

void Renderer::InitShader()
{
    ShaderSource* mainShaderVertex = ResourceManager::Get<ShaderSource>("main.vert");
    ShaderSource* mainShaderFrag = ResourceManager::Get<ShaderSource>("main.frag");

    PC_CORE::ProgramShaderCreateInfo createInfo =
        {
        .prograShaderName = mainShader,
        .programShaderFlag = 0
        };

    ShaderSourceAndPath vertexData =
        {
        .shaderSourceCode = mainShaderVertex->GetShaderSourceFile(),
        .shaderSourceCodePath = mainShaderVertex->path.generic_string().c_str()
        };

    ShaderSourceAndPath framgmentData =
        {
        .shaderSourceCode = mainShaderFrag->GetShaderSourceFile(),
        .shaderSourceCodePath = mainShaderFrag->path.generic_string().c_str()
        };

    std::vector<ShaderSourceAndPath> sourceAndPaths =
        {
        vertexData,
        framgmentData
        };
    
    RHI::CreateShader(createInfo, {vertexData, framgmentData});
    
}
