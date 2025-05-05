#include "rendering/render_passes/forward_pass.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::ForwardPass::ForwardPass()
{
    m_renderPassHandle = Rhi::CreateRenderPass(PC_CORE::RHIFormat::R8G8B8A8_UNORM, PC_CORE::RHIFormat::D32_SFLOAT);

}

void PC_CORE::ForwardPass::Build()
{
}

void PC_CORE::ForwardPass::Execute()
{
    /*
    UpdateCameraUniformBuffer(renderingContext);
    UpdateViewExtremumBuffer(renderingContext);

    ClearValueFlags clearValueFlags = static_cast<ClearValueFlags>(ClearValueFlags::ClearValueColor |
        ClearValueFlags::ClearValueDepth);
    const BeginRenderPassInfo beginRenderPassInfo =
    {
        .renderPass = forwardPass,
        // shpoulmd be gbuffer
        .frameBuffer = gbuffers->GetFrameBuffer(),
        .renderOffSet = {0, 0},
        .extent = {renderingContext.renderingContextSize.x, renderingContext.renderingContextSize.y},
        .clearValueFlags = clearValueFlags,
        .clearColor = {0, 0, 0, 0.f},
        .clearDepth = 1.f
    };

    primaryCommandList->BeginRenderPass(beginRenderPassInfo);
    primaryCommandList->BindProgram(m_ForwardShader);

    const ViewportInfo viewportInfo =
    {
        .transform = {0, 0},
        .size = {
            static_cast<float>(renderingContext.renderingContextSize.x),
            static_cast<float>(renderingContext.renderingContextSize.y)
        },
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .scissorsOff = {0, 0},
        .scissorsextent = {renderingContext.renderingContextSize.x, renderingContext.renderingContextSize.y}
    };
    primaryCommandList->SetViewPort(viewportInfo);
    primaryCommandList->BindDescriptorSet(m_ForwardShader, m_ShaderProgramDescriptorSet, 0, 1);


    std::function<void(Transform&, StaticMesh&)> func = std::bind(&Renderer::DrawStaticMesh, this,
                                                                  std::placeholders::_1, std::placeholders::_2);
    _world->entityManager.ForEach<Transform, StaticMesh>(func);
    primaryCommandList->EndRenderPass();*/
}
