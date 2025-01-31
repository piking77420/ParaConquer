#include "rendering/renderer.hpp"

#include "low_renderer/rhi.hpp"
#include "low_renderer/vertex.hpp"
#include "resources/resource_manager.hpp"
#include <thread>
#include "time/core_time.hpp"
#include "math/matrix_transformation.hpp"

using namespace PC_CORE;

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.5f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint32_t> indices = {
    0, 1, 2, 2, 3, 0
};


void Renderer::Init()
{
    m_RhiContext = Rhi::GetRhiContext();

    constexpr RasterizerInfo rasterizerInfo =
    {
        .polygonMode = PolygonMode::Fill,
        .cullModeFlag = CullModeFlagBit::Back,
        .frontFace = FrontFace::CounterClockwise
    };


    const ShaderGraphicPointInfo shaderGraphicPointInfo =
    {
        .rasterizerInfo = rasterizerInfo,
        .vertexInputBindingDescritions = {Vertex::GetBindingDescrition(0)},
        .vertexAttributeDescriptions = Vertex::GetAttributeDescriptions(0)
    };

    const std::vector<std::pair<ShaderStageType, std::string>> source =
    {
        {
            ShaderStageType::VERTEX,
            "main_spv.vert"
        },
        {
            ShaderStageType::FRAGMENT,
            "main_spv.frag"
        }
    };

    const ShaderInfo shaderInfo =
    {
        .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
        .shaderInfoData = shaderGraphicPointInfo,
        .shaderSources = source
    };

    const PC_CORE::ProgramShaderCreateInfo triangleCreateInfo =
    {
        .prograShaderName = "Main",
        .shaderInfo = shaderInfo,
        .renderPass = RhiContext::GetContext().swapChain->GetSwapChainRenderPass(),
    };


    m_Main = PC_CORE::Rhi::CreateShader(triangleCreateInfo);

    constexpr CommandListCreateInfo commandListCreateInfo =
    {
        ._commandPoolFamily = CommandPoolFamily::Graphics
    };

    m_CommandList = PC_CORE::Rhi::CreateCommandList(commandListCreateInfo);

    m_VertexBuffer = std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
    m_IndexBuffer = std::make_shared<IndexBuffer>(indices.data(), indices.size() * sizeof(indices[0]));

    m_Main->AllocDescriptorSet(&m_ShaderProgramDescriptorSet);

    m_UniformBuffer = UniformBuffer(&sceneBufferGPU, sizeof(sceneBufferGPU));
    
    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                0,
                &m_UniformBuffer
            },
        };

    m_ShaderProgramDescriptorSet->WriteDescriptorSets(descriptorSets);
}

bool Renderer::BeginDraw(Window* _window)
{
    return m_RhiContext->swapChain->GetSwapChainImageIndex(_window);
}

void Renderer::Render()
{
    const size_t currentFrame = Rhi::GetFrameIndex();


    sceneBufferGPU.time = PC_CORE::Time::GetTime();
    sceneBufferGPU.deltatime = PC_CORE::Time::DeltaTime();
    Tbx::Trs3D<float>(Tbx::Vector3f{}, Tbx::Vector3f(0, 0, 90.f * Deg2Rad * sceneBufferGPU.time),
                      Tbx::Vector3f(1, 1, 1), &sceneBufferGPU.model);
    sceneBufferGPU.view = Tbx::LookAtRH<float>(Tbx::Vector3f(2, 2, 2), Tbx::Vector3f(), Tbx::Vector3f::UnitZ());
    sceneBufferGPU.proj = Tbx::PerspectiveMatrixFlipYAxis<float>(45.f * Deg2Rad, 1920.f / 1080.f, 0.1, 1000.f);

    m_UniformBuffer.Update(&sceneBufferGPU, sizeof(sceneBufferGPU));

    m_CommandList->Reset();
    m_CommandList->BeginRecordCommands();

    uint32_t swapChainWidht = m_RhiContext->swapChain->GetWidth();
    uint32_t swapChainHeight = m_RhiContext->swapChain->GetHeight();

    const BeginRenderPassInfo BeginRenderPassInfo =
    {
        .renderPass = RhiContext::GetContext().swapChain->GetSwapChainRenderPass(),
        .frameBuffer = RhiContext::GetContext().swapChain->GetFrameBuffer(),
        .renderOffSet = {0, 0},
        .extent = {swapChainWidht, swapChainHeight},
        .clearColor = {0, 0, 0, 1.f}
    };
    m_CommandList->BeginRenderPass(BeginRenderPassInfo);
    m_CommandList->BindProgram(m_Main);

    const ViewportInfo viewportInfo =
    {
        .transform = {0, 0},
        .size = {static_cast<float>(swapChainWidht), static_cast<float>(swapChainHeight)},
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .scissorsOff = {0, 0},
        .scissorsextent = {swapChainWidht, swapChainHeight}
    };
    m_CommandList->SetViewPort(viewportInfo);
    m_CommandList->BindVertexBuffer(*m_VertexBuffer, 0, 1);
    m_CommandList->BindIndexBuffer(*m_IndexBuffer, 0);
    m_CommandList->BindDescriptorSet(m_Main, m_ShaderProgramDescriptorSet, 0, 1);

    m_CommandList->DrawIndexed(m_IndexBuffer->GetIndexCount(), 1, 0, 0, 0);
    m_CommandList->EndRenderPass();
    m_CommandList->EndRecordCommands();
}


void Renderer::SwapBuffers(Window* _window)
{
    m_RhiContext->swapChain->Present(m_CommandList.get(), _window);
    Rhi::NextFrame();
}
