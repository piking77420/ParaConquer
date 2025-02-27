#include "rendering/renderer.hpp"

#include "low_renderer/rhi.hpp"
#include "low_renderer/vertex.hpp"
#include "resources/resource_manager.hpp"
#include <thread>
#include "time/core_time.hpp"
#include "math/matrix_transformation.hpp"
#include "rendering/light.hpp"
#include "resources/texture.hpp"
#include "world/static_mesh.hpp"
#include "world/transform.hpp"
#include "rendering/render_passes/render_pass.hpp"

using namespace PC_CORE;


void Renderer::Init()
{
    m_RhiContext = Rhi::GetRhiContext();
    sceneLightsBuffer = std::make_unique<SceneLightsBuffer>();
    forwardPass = Rhi::CreateRenderPass(PC_CORE::RHIFormat::R8G8B8A8_UNORM, PC_CORE::RHIFormat::D32_SFLOAT);
    drawTextureScreenQuadPass = Rhi::CreateRenderPass(PC_CORE::RHIFormat::R8G8B8A8_UNORM);
    forwardPass = Rhi::CreateRenderPass(PC_CORE::RHIFormat::R8G8B8A8_UNORM, PC_CORE::RHIFormat::D32_SFLOAT);

    CreateForwardShader();
    CreateDrawQuadShader();

    constexpr CommandListCreateInfo commandListCreateInfo =
    {
        ._commandPoolFamily = CommandPoolFamily::Graphics
    };

    primaryCommandList = PC_CORE::Rhi::CreateCommandList(commandListCreateInfo);
    m_ForwardShader->AllocDescriptorSet(&m_ShaderProgramDescriptorSet, 0);

    cameraUniformBuffer = UniformBuffer(&sceneBufferGPU, sizeof(sceneBufferGPU));


    UniformBufferDescriptor uniformBufferDescriptor
    {
        .buffer = &cameraUniformBuffer,
    };

    UniformBufferDescriptor lightData
    {
        .buffer = &sceneLightsBuffer
        ->uniformBuffer,
    };

    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets =
    {
        {
            ShaderProgramDescriptorType::UniformBuffer,
            0,
            &uniformBufferDescriptor,
            nullptr,
        },
        {
            ShaderProgramDescriptorType::UniformBuffer,
            1,
            &lightData,
            nullptr,
        }
    };

    m_ShaderProgramDescriptorSet->WriteDescriptorSets(descriptorSets);


    m_ViewExtrmumUniformBuffer = UniformBuffer(&m_ViewExtremum, sizeof(m_ViewExtremum));

    InitRenderSystem();
}

void Renderer::Destroy()
{
    primaryCommandList = nullptr;
    m_ForwardShader = nullptr;
    m_DrawTextureScreenQuadShader = nullptr;
    forwardPass = nullptr;
    drawTextureScreenQuadPass = nullptr;
}


bool Renderer::BeginDraw(Window* _window)
{
    bool hasObtainSwapChian = m_RhiContext->swapChain->GetSwapChainImageIndex(_window);
    if (hasObtainSwapChian)
    {
        primaryCommandList->Reset();
        primaryCommandList->BeginRecordCommands();
    }

    QueryWorldData(World::GetWorld());
    sceneLightsBuffer
        ->Fecth();

    return hasObtainSwapChian;
}

void Renderer::UpdateCameraUniformBuffer(const PC_CORE::RenderingContext& renderingContext)
{
    currentRenderingContext = &renderingContext;
    SceneBufferGPU& sceneBufferGpu = sceneBufferGPU;

    sceneBufferGpu.time = PC_CORE::Time::GetTime();
    sceneBufferGPU.deltatime = PC_CORE::Time::DeltaTime();
    Tbx::Matrix4x4f view = Tbx::LookAtRH<float>(Tbx::Vector3f::Zero(),
                                                renderingContext.lowLevelCamera.front, renderingContext.lowLevelCamera.up);
    Tbx::Matrix4x4f projection = Tbx::PerspectiveMatrixFlipYAxis<float>(renderingContext.lowLevelCamera.fov,
                                                                        renderingContext.lowLevelCamera.aspect,
                                                                        renderingContext.lowLevelCamera.near,
                                                                        renderingContext.lowLevelCamera.far);
    sceneBufferGPU.vp = projection * view;
    sceneBufferGPU.view = view;
    sceneBufferGPU.proj = projection;


    Tbx::Invert(sceneBufferGPU.view, &sceneBufferGPU.viewInv);
    Tbx::Invert(sceneBufferGPU.proj, &sceneBufferGPU.projInv);
    Tbx::Invert(sceneBufferGPU.vp, &sceneBufferGPU.vpInv);

    cameraUniformBuffer.Update(&sceneBufferGPU, sizeof(sceneBufferGPU));
}

void Renderer::UpdateViewExtremumBuffer(const PC_CORE::RenderingContext& renderingContext)
{
    constexpr float far = 1.f;
    constexpr float near = 0.2f;

    // TO DO FIX INVET + ADD GIZOMO FOR DEBUG
    Tbx::Vector4f topLeftB = sceneBufferGPU.vpInv * Tbx::Vector4f{-1.f, -1.f, far, 1.f};
    Tbx::Vector4f topRightB = sceneBufferGPU.vpInv * Tbx::Vector4f{1.f, -1.f, far, 1.f};
    Tbx::Vector4f bottomLeftB = sceneBufferGPU.vpInv * Tbx::Vector4f{-1.f, 1.f, far, 1.f};
    Tbx::Vector4f bottomRightB = sceneBufferGPU.vpInv * Tbx::Vector4f{1.f, 1.f, far, 1.f};

    topLeftB /= topLeftB.w;
    topRightB /= topRightB.w;
    bottomLeftB /= bottomLeftB.w;
    bottomRightB /= bottomRightB.w;

    Tbx::Vector4f topLeftA = sceneBufferGPU.vpInv * Tbx::Vector4f{-1.f, -1.f, near, 1.f};
    Tbx::Vector4f topRightA = sceneBufferGPU.vpInv * Tbx::Vector4f{1.f, -1.f, near, 1.f};
    Tbx::Vector4f bottomLeftA = sceneBufferGPU.vpInv * Tbx::Vector4f{-1.f, 1.f, near, 1.f};
    Tbx::Vector4f bottomRightA = sceneBufferGPU.vpInv * Tbx::Vector4f{1.f, 1.f, near, 1.f};

    topLeftA /= topLeftA.w;
    topRightA /= topRightA.w;
    bottomLeftA /= bottomLeftA.w;
    bottomRightA /= bottomRightA.w;

    const Tbx::Vector3f topLeft3 = {topLeftB.x - topLeftA.x, topLeftB.y - topLeftA.y, topLeftB.z - topLeftA.y};
    const Tbx::Vector3f topRight3 = {topRightB.x - topRightA.x, topRightB.y - topRightA.y, topRightB.z - topRightA.z};
    const Tbx::Vector3f bottomLeft3 = {
        bottomLeftB.x - bottomLeftA.x, bottomLeftB.y - bottomLeftA.y, bottomLeftB.z - bottomLeftA.z
    };
    const Tbx::Vector3f bottomRight3 = {
        bottomRightB.x - bottomRightA.x, bottomRightB.y - bottomRightA.y, bottomRightB.z - bottomRightA.z
    };

    m_ViewExtremum =
    {
        .topLeft = topLeft3 ,
        .topRight = topRight3,
        .bottomLeft = bottomLeft3 + renderingContext.lowLevelCamera.position,
        .bottomRight = bottomRight3 + renderingContext.lowLevelCamera.position
    };

    m_ViewExtrmumUniformBuffer.Update(&m_ViewExtremum, sizeof(m_ViewExtremum));
}


void Renderer::DrawToRenderingContext(const PC_CORE::RenderingContext& renderingContext, Gbuffers* gbuffers,
                                      World* _world)
{
    m_CurrentWorld = _world;

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
    primaryCommandList->BindProgram(m_ForwardShader.get());

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
    primaryCommandList->BindDescriptorSet(m_ForwardShader.get(), m_ShaderProgramDescriptorSet, 0, 1);


    //std::function<void(Transform&, StaticMesh&)> func = std::bind(&Renderer::DrawStaticMesh, this,
      //                                                            std::placeholders::_1, std::placeholders::_2);
   // _world->entityManager.ForEach<Transform, StaticMesh>(func);

    for (auto& it : rendererSystem->m_SignatureEntitiesSet[rendererSystem->staticMeshSignature])
    {
        DrawStaticMesh(World::GetWorld()->GetComponent<Transform>(it),
            World::GetWorld()->GetComponent<StaticMesh>(it));
    }

    primaryCommandList->EndRenderPass();

    std::shared_ptr<PC_CORE::SwapChain> swapChain = RhiContext::GetContext().swapChain;

    const BeginRenderPassInfo drawToViewport =
    {
        .renderPass = drawTextureScreenQuadPass,
        .frameBuffer = renderingContext.finalImageFrameBuffer,
        .renderOffSet = {0, 0},
        .extent = {renderingContext.renderingContextSize.x, renderingContext.renderingContextSize.y},
        .clearValueFlags = static_cast<ClearValueFlags>(ClearValueFlags::ClearValueColor),
        .clearColor = {0, 0, 0, 1.f},
        .clearDepth = 0.f,
        .clearStencil = 0.f
    };
    primaryCommandList->BeginRenderPass(drawToViewport);
    primaryCommandList->BindProgram(m_DrawTextureScreenQuadShader.get());
    DrawTextureScreenQuad(*renderingContext.viewPortDescriptorSet);
    primaryCommandList->EndRenderPass();
}


void Renderer::SwapBuffers(Window* _window)
{
    std::shared_ptr<PC_CORE::SwapChain> swapChain = RhiContext::GetContext().swapChain;
    swapChain->BeginSwapChainRenderPass(primaryCommandList.get());
    primaryCommandList->ExucuteFetchCommand();
    swapChain->EndSwapChainRenderPass(primaryCommandList.get());
    primaryCommandList->EndRecordCommands();

    m_RhiContext->swapChain->Present(primaryCommandList.get(), _window);
    Rhi::NextFrame();
}

void Renderer::DrawTextureScreenQuad(const ShaderProgramDescriptorSets& _ShaderProgramDescriptorSets)
{
    primaryCommandList->BindDescriptorSet(m_DrawTextureScreenQuadShader.get(), &_ShaderProgramDescriptorSets, 0, 1);
    primaryCommandList->Draw(4, 1, 0, 0);
}

void Renderer::QueryWorldData(World* world)
{
    for (auto& it : rendererSystem->m_SignatureEntitiesSet[rendererSystem->dirLightSignature])
    {
        QueryLightDirData(world->GetComponent<DirLight>(it), world->GetComponent<Transform>(it));
    }

}

void Renderer::QueryLightDirData(DirLight& dirLight, Transform& transform)
{
    sceneLightsBuffer
        ->sceneLightData.ambiant = dirLight.color;
    sceneLightsBuffer
        ->sceneLightData.color = dirLight.ambiant;
    sceneLightsBuffer
        ->sceneLightData.intensity = dirLight.intensity;
    sceneLightsBuffer
        ->sceneLightData.direction = transform.rotation.quaternion.ToEulerAngles().Normalize();
}

void Renderer::CreateForwardShader()
{
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
        .vertexAttributeDescriptions = Vertex::GetAttributeDescriptions(0),
        .enableDepthTest = true,
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
        .renderPass = forwardPass,
    };


    m_ForwardShader = PC_CORE::Rhi::CreateShader(triangleCreateInfo);
}

void Renderer::CreateDrawQuadShader()
{
    constexpr RasterizerInfo rasterizerInfo =
    {
        .polygonMode = PolygonMode::Fill,
        .cullModeFlag = CullModeFlagBit::Back,
        .frontFace = FrontFace::CounterClockwise
    };


    const ShaderGraphicPointInfo shaderGraphicPointInfo =
    {
        .rasterizerInfo = rasterizerInfo,
        .vertexInputBindingDescritions = {},
        .vertexAttributeDescriptions = {},
        .enableDepthTest = false,
    };

    const std::vector<std::pair<ShaderStageType, std::string>> source =
    {
        {
            ShaderStageType::VERTEX,
            "draw_texture_screen_quad_spv.vert"
        },
        {
            ShaderStageType::FRAGMENT,
            "draw_texture_screen_quad_spv.frag"
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
        .prograShaderName = "DrawTextureScreenQuad",
        .shaderInfo = shaderInfo,
        .renderPass = drawTextureScreenQuadPass,
    };


    m_DrawTextureScreenQuadShader = PC_CORE::Rhi::CreateShader(triangleCreateInfo);
}

void Renderer::DrawStaticMesh(PC_CORE::Transform& _transform, PC_CORE::StaticMesh& _staticMesh)
{
    if (_staticMesh.material == nullptr || _staticMesh.mesh == nullptr)
        return;

    // Compute Matrix
    Tbx::Matrix4x4d modelMatrixd[2];
    Tbx::Trs3D<double>(_transform.position - currentRenderingContext->lowLevelCamera.position, _transform.rotation.quaternion,
                       _transform.scale, &modelMatrixd[0]);
    Tbx::Invert<double>(modelMatrixd[0], &modelMatrixd[1]);
    modelMatrixd[1] = modelMatrixd[1].Transpose();

    Tbx::Matrix4x4f modelMatrixf[2];
    modelMatrixf[0] = static_cast<Tbx::Matrix4x4<float>>(modelMatrixd[0]);
    modelMatrixf[1] = static_cast<Tbx::Matrix4x4<float>>(modelMatrixd[1]);

    // Send Data
    primaryCommandList->BindDescriptorSet(m_ForwardShader.get(), _staticMesh.material->GetDescriptorSet(), 1, 1);
    primaryCommandList->PushConstant(m_ForwardShader.get(), "PushConstants", &modelMatrixf,
                                     sizeof(Tbx::Matrix4x4f) * 2);
    primaryCommandList->BindVertexBuffer(_staticMesh.mesh->vertexBuffer, 0, 1);
    primaryCommandList->BindIndexBuffer(_staticMesh.mesh->indexBuffer, 0);
    primaryCommandList->DrawIndexed(_staticMesh.mesh->indexBuffer.GetIndexCount(), 1, 0, 0, 0);
}

void Renderer::AtmoSpherePass()
{
}

void Renderer::InitRenderSystem()
{
    rendererSystem = World::GetWorld()->RegisterSystem<RendererSystem>();
    
    rendererSystem->staticMeshSignature.set(World::GetWorld()->GetComponentTypeBit<Transform>(),true);
    rendererSystem->staticMeshSignature.set(World::GetWorld()->GetComponentTypeBit<StaticMesh>(), true);
    rendererSystem->AddSignature(rendererSystem->staticMeshSignature);

    rendererSystem->dirLightSignature.set(World::GetWorld()->GetComponentTypeBit<Transform>(), true);
    rendererSystem->dirLightSignature.set(World::GetWorld()->GetComponentTypeBit<DirLight>(), true);
    rendererSystem->AddSignature(rendererSystem->dirLightSignature);

}
