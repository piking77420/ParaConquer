#include "rendering/renderer.hpp"

#include "low_renderer/rhi.hpp"
#include "low_renderer/vertex.hpp"
#include "resources/resource_manager.hpp"
#include <thread>
#include "time/core_time.hpp"
#include "math/matrix_transformation.hpp"
#include "rendering/light.hpp"
#include "resources/texture_2d.hpp"
#include "world/static_mesh.hpp"
#include "world/transform.hpp"
#include "rendering/render_passes/render_pass.hpp"

#include <perf_region.hpp>

#include "math/toolbox_typedef.hpp"

using namespace PC_CORE;


void Renderer::Init()
{
    PERF_REGION_SCOPED;

    m_RhiContext = Rhi::GetRhiContext();
    constexpr CommandListCreateInfo commandListCreateInfo =
    {
        ._commandPoolFamily = CommandPoolFamily::Graphics
    };

    primaryCommandList = PC_CORE::Rhi::CreateCommandList(commandListCreateInfo);
    
    sceneLightsBuffer = std::make_unique<SceneLightsBuffer>();
    CreateForwardRenderPass();
    drawTextureScreenQuadPass = Rhi::CreateRenderPass(PC_CORE::RHIFormat::R8G8B8A8_UNORM, Rhi::GetRhiContext()->physicalDevices->GetPhysicalDevice().GetMaxUsableSampleCount());

    CreateForwardShader();
    CreateDrawQuadShader();
    CreateSkyRenderingShader();
    
    cameraUniformBuffer = UniformBuffer(&sceneBufferGPU, sizeof(sceneBufferGPU), BufferMemoryUsage::Dynamic);


    UniformBufferDescriptor cameraBufferDescritptor
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
            CAMERA_BINDING,
            & cameraBufferDescritptor,
            nullptr,
        },
        {
            ShaderProgramDescriptorType::UniformBuffer,
            LIGHTDATA_BINDING,
            &lightData,
            nullptr,
        }
    };

    m_ForwardShader->AllocDescriptorSet(&m_ShaderProgramDescriptorSet, 0);
    m_ShaderProgramDescriptorSet->WriteDescriptorSets(descriptorSets);
    

    descriptorSets =
    {
        {
            ShaderProgramDescriptorType::UniformBuffer,
            CAMERA_BINDING,
            & cameraBufferDescritptor,
            nullptr,
        },
    };
    InitRenderSystem();

}
void Renderer::Destroy()
{
   
    // TODO(avoir make shader a shared ptr or remove acquire beacause of resoure manager) Release Shader
    m_ForwardShader = nullptr;
    m_DrawTextureScreenQuadShader = nullptr;
}


void Renderer::BeginDraw(Window* _window)
{
    PERF_REGION_SCOPED;

    m_RhiContext->swapChain->GetSwapChainImageIndex(_window);


    primaryCommandList->Reset();
    primaryCommandList->BeginRecordCommands();

    
    QueryWorldData(World::GetWorld());
    sceneLightsBuffer
        ->Fecth();
        
}

void Renderer::UpdateCameraUniformBuffer(const PC_CORE::RenderingContext& renderingContext)
{
    PERF_REGION_SCOPED;

    
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
    sceneBufferGPU.viewInv = sceneBufferGPU.view.Invert();
    sceneBufferGPU.projInv = sceneBufferGPU.proj.Invert();
    sceneBufferGPU.vpInv = sceneBufferGPU.vp.Invert();
    sceneBufferGPU.cameraNear = renderingContext.lowLevelCamera.near;
    sceneBufferGPU.cameraFar = renderingContext.lowLevelCamera.far;

    cameraUniformBuffer.Update(&sceneBufferGPU, sizeof(sceneBufferGPU));
}

void Renderer::UpdateViewExtremumBuffer(const PC_CORE::RenderingContext& renderingContext)
{
    /*
    PERF_REGION_SCOPED;
    const float planeHeight = sceneBufferGPU.cameraNear * std::tan(renderingContext.lowLevelCamera.fov * 0.5f) * 2.f;
    const float planeWidth = planeHeight * renderingContext.lowLevelCamera.aspect;

    Tbx::Matrix4x4f view = Tbx::LookAtRH<float>(
        renderingContext.lowLevelCamera.position,
        renderingContext.lowLevelCamera.position + renderingContext.lowLevelCamera.front,
        renderingContext.lowLevelCamera.up
    );

    float scatteringStrenght = 20.f;
    Tbx::Vector3f waveLenght = Tbx::Vector3f{700,530,400};
    float scatterR = std::pow(400.f / waveLenght.x,4.f) * scatteringStrenght;
    float scatterG = std::pow(400.f / waveLenght.y,4.f) * scatteringStrenght;
    float scatterB = std::pow(400.f / waveLenght.z,4.f) * scatteringStrenght;

    
    m_AtomsphereBuffer =
    {
        .camToWorldMatrix = view.Invert(),
        .viewParam = Tbx::Vector3f(planeWidth, planeHeight, -renderingContext.lowLevelCamera.near),
        .cameraPos = renderingContext.lowLevelCamera.position,
        .sunDir = -sceneLightsBuffer->sceneLightData.direction, // negate is important
        .sunColor = sceneLightsBuffer->sceneLightData.color,
        .scatteringCoeff = Tbx::Vector3f(scatterR, scatterG, scatterB)
    };

    m_AtmosphereUniformBuffer.Update(&m_AtomsphereBuffer, sizeof(m_AtomsphereBuffer));*/
}


void Renderer::DrawToRenderingContext(const PC_CORE::RenderingContext& renderingContext,
                                      World* _world)
{
    
    PERF_REGION_SCOPED;

    m_CurrentWorld = _world;
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

    UpdateCameraUniformBuffer(renderingContext);
    UpdateViewExtremumBuffer(renderingContext);

    
    ClearValueFlags clearValueFlags = static_cast<ClearValueFlags>(ClearValueFlags::ClearValueColor |
        ClearValueFlags::ClearValueDepth);

    std::array<Tbx::Vector4f, 1> clearValues =  {
        Tbx::Vector4f(0, 0, 0, 0.f),
        };
    
    const BeginRenderPassInfo beginRenderPassInfo =
    {
        .renderPass = forwardPass,
        .frameBuffer = renderingContext.gbufferFrameBuffer,
        .renderOffSet = {0, 0},
        .extent = {renderingContext.renderingContextSize.x, renderingContext.renderingContextSize.y},
        .clearValueFlags = clearValueFlags,
        .clearColor = clearValues.data(),
        .clearValueCount = clearValues.size(),
        .clearDepth = 1.f
    };

    primaryCommandList->BeginDebugLabel("Begin Forward Pass", FORWARD_DEBUG_COLOR);
    primaryCommandList->BeginRenderPass(beginRenderPassInfo);
    primaryCommandList->BindProgram(m_ForwardShader.get());

    
    primaryCommandList->SetViewPort(viewportInfo);
    primaryCommandList->BindDescriptorSet(m_ForwardShader.get(), m_ShaderProgramDescriptorSet, SCENE_DESCRIPTOR_SET, 1);

    primaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleList);

    Level& level = World::GetWorld()->level;
    // draw all static mesh
    for (const auto& it : *rendererSystem->GetEntityIdList(rendererSystem->staticMeshSignature))
        DrawStaticMesh(level.GetComponent<Transform>(it),
            level.GetComponent<StaticMesh>(it));
    

    // draw the sky
   // DrawSky();
    primaryCommandList->EndRenderPass();
    
    primaryCommandList->EndDebugLabel();

    std::array<Tbx::Vector4f, 2> clearValues2 =  {
        Tbx::Vector4f(0, 0, 0, 0.f),
        Tbx::Vector4f(0, 0, 0, 0.f),
        };
    
    const BeginRenderPassInfo drawToViewport =
    {
        .renderPass = drawTextureScreenQuadPass,
        .frameBuffer = renderingContext.finalImageFrameBuffer,
        .renderOffSet = {0, 0},
        .extent = {renderingContext.renderingContextSize.x, renderingContext.renderingContextSize.y},
        .clearValueFlags = static_cast<ClearValueFlags>(ClearValueFlags::ClearValueColor),
        .clearColor = clearValues2.data(),
        .clearValueCount = clearValues2.size(),
        .clearDepth = 0.f,
        .clearStencil = 0.f
    };


    
    primaryCommandList->BeginDebugLabel("Final Pass", FINAL_RENDER_PASS_DEBUG_COLOR);           
    primaryCommandList->BeginRenderPass(drawToViewport);
    primaryCommandList->SetViewPort(viewportInfo);
    primaryCommandList->BindProgram(m_DrawTextureScreenQuadShader.get());
    primaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleStrip );
    DrawTextureScreenQuad(*renderingContext.viewPortDescriptorSet);
    primaryCommandList->EndRenderPass();
    primaryCommandList->EndDebugLabel();
}


void Renderer::SwapBuffers(Window* _window)
{
    PERF_REGION_SCOPED;

    
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
    PERF_REGION_SCOPED;
    
    Level& level = world->level;
    for (const auto& it : *rendererSystem->GetEntityIdList(rendererSystem->dirLightSignature))
    {
        QueryLightDirData(level.GetComponent<DirLight>(it), level.GetComponent<Transform>(it));
    }

}

void Renderer::QueryLightDirData(DirLight& dirLight, Transform& transform)
{
    sceneLightsBuffer
        ->sceneLightData.ambiant = dirLight.ambiant;
    sceneLightsBuffer
        ->sceneLightData.color = dirLight.color;
    sceneLightsBuffer
        ->sceneLightData.intensity = dirLight.intensity;
    sceneLightsBuffer
        ->sceneLightData.direction =  Tbx::Quaterniond::ToEulerAngles(transform.rotation.quaternion).Normalize();
}

void Renderer::CreateForwardShader()
{
    PERF_REGION_SCOPED;

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
        .shaderInfo = shaderInfo,
        .renderPass = forwardPass,
    };


    m_ForwardShader = ResourceManager::Create<ShaderProgram>("ForwardShader", triangleCreateInfo);
}

void Renderer::CreateDrawQuadShader()
{
    PERF_REGION_SCOPED;

    const RasterizerInfo rasterizerInfo =
    {
        .polygonMode = PolygonMode::Fill,
        .cullModeFlag = CullModeFlagBit::None,
        .frontFace = FrontFace::CounterClockwise,
        .multiSampleRasterization = Rhi::GetRhiContext()->physicalDevices->GetPhysicalDevice().GetMaxUsableSampleCount()
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
        .shaderInfo = shaderInfo,
        .renderPass = drawTextureScreenQuadPass,
    };

    m_DrawTextureScreenQuadShader = ResourceManager::Create<ShaderProgram>("DrawQuadShader", triangleCreateInfo);
}

void Renderer::CreateSkyRenderingShader()
{
    /*
    PERF_REGION_SCOPED;

    const RasterizerInfo rasterizerInfo =
    {
        .polygonMode = PolygonMode::Fill,
        .cullModeFlag = CullModeFlagBit::None,
        .frontFace = FrontFace::CounterClockwise,
    };


    const ShaderGraphicPointInfo shaderGraphicPointInfo =
    {
        .rasterizerInfo = rasterizerInfo,
        .depthCompareOp = CompareOp::LESS_OR_EQUAL,
        .vertexInputBindingDescritions = {},
        .vertexAttributeDescriptions = {},
        .enableDepthTest = true,
    };

    const std::vector<std::pair<ShaderStageType, std::string>> source =
    {
        {
            ShaderStageType::VERTEX,
            "sky_rendering_spv.vert"
        },
        {
            ShaderStageType::FRAGMENT,
            "sky_rendering_spv.frag"
        }
    };

    const ShaderInfo shaderInfo =
    {
        .shaderProgramPipelineType = shaderProgramPipelineType::POINT_GRAPHICS,
        .shaderInfoData = shaderGraphicPointInfo,
        .shaderSources = source
    };

    const PC_CORE::ProgramShaderCreateInfo triangleCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = forwardPass,
    };

    m_SkyRenderingShader = PC_CORE::Rhi::CreateRhiShaderProgram(triangleCreateInfo);
    
    m_AtmosphereUniformBuffer = RhiUniformBuffer(&m_AtomsphereBuffer, sizeof(m_AtomsphereBuffer));*/
}

void Renderer::DrawStaticMesh(PC_CORE::Transform& _transform, PC_CORE::StaticMesh& _staticMesh)
{
    PERF_REGION_SCOPED;

    if (_staticMesh.material.expired() || _staticMesh.mesh.expired())
        return;

    
    // Compute Matrix
    Tbx::Matrix4x4d modelMatrixd[2];

    modelMatrixd[0] = Tbx::Trs4x4<float>(_transform.position - currentRenderingContext->lowLevelCamera.position, _transform.rotation.quaternion,
        _transform.scale);

    modelMatrixd[1] = modelMatrixd[0].Invert().Transpose();

    Tbx::Matrix4x4f modelMatrixf[2];
    modelMatrixf[0] = modelMatrixd[0];
    modelMatrixf[1] = modelMatrixd[1];

    Material* material = _staticMesh.material.lock().get();
    Mesh* mesh = _staticMesh.mesh.lock().get();

    // Send Data
    primaryCommandList->BindDescriptorSet(m_ForwardShader.get(), material->GetDescriptorSet(), MATERIAL_DESCRIPTOR_SET, 1);
    primaryCommandList->PushConstant(m_ForwardShader.get(), "PushConstants", &modelMatrixf,
                                     sizeof(Tbx::Matrix4x4f) * 2);
    primaryCommandList->BindVertexBuffer(*mesh->vertexBuffer.GetRhiBuffer(), 0, 1);
    primaryCommandList->BindIndexBuffer(*mesh->indexBuffer.GetRhiBuffer(), 0);
    primaryCommandList->DrawIndexed(mesh->indexBuffer.GetIndexCount(), 1, 0, 0, 0);
}

void Renderer::DrawSky()
{
    
    primaryCommandList->BindProgram(m_SkyRenderingShader.get());
    primaryCommandList->BindDescriptorSet(m_SkyRenderingShader.get(), m_ShaderProgramDescriptorSetsSky, SCENE_DESCRIPTOR_SET, 1);
    primaryCommandList->SetPrimitiveTopology(PC_CORE::PrimitiveTopology::PrimitiveTopologyTriangleStrip);
    primaryCommandList->Draw(4, 1, 0, 0);

}

void Renderer::InitRenderSystem()
{
    PERF_REGION_SCOPED;
    Level& level = World::GetWorld()->level;

    rendererSystem = level.RegisterSystem<RendererSystem>();
    
    rendererSystem->staticMeshSignature.set(level.GetComponentTypeBit<Transform>(),true);
    rendererSystem->staticMeshSignature.set(level.GetComponentTypeBit<StaticMesh>(), true);
    rendererSystem->AddSignature(rendererSystem->staticMeshSignature);

    rendererSystem->dirLightSignature.set(level.GetComponentTypeBit<Transform>(), true);
    rendererSystem->dirLightSignature.set(level.GetComponentTypeBit<DirLight>(), true);
    rendererSystem->AddSignature(rendererSystem->dirLightSignature);

}

void Renderer::CreateForwardRenderPass()
{
    std::vector<RenderPassAttachementDescriptor> colorAttachement;
    colorAttachement.resize(1);

    colorAttachement[0] =
        {
        .attachmentType = AttachmentType::Color,
        .format = PC_CORE::RHIFormat::R8G8B8A8_UNORM,
        .sampleCount = 1,
        .load = LoadOperation::Clear,
        .store = StoreOperation::Store,
        .stencilLoad = LoadOperation::DontCare,
        .stencilStore = StoreOperation::DontCare,
        };

    RenderPassAttachementDescriptor depthAttachement =
     {
        .attachmentType = AttachmentType::Depth,
        .format = PC_CORE::RHIFormat::D32_SFLOAT,
        .sampleCount = 1,
        .load = LoadOperation::Clear,
        .store = StoreOperation::Store,
        .stencilLoad = LoadOperation::DontCare,
        .stencilStore = StoreOperation::DontCare,
        };

    std::vector<SubPassDescription> subPassDescriptions;
    subPassDescriptions.resize(1);

    subPassDescriptions[0] =
        {
        .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
        .colorAttachementDescriptorIndicies = {0},
        .subPassDependcies =
        {
            .srcStageMask = static_cast<PipelineStageFlags>(
                PipelineStageFlagBits::ColorAttachmentOutput | PipelineStageFlagBits::EarlyFragmentTests
            ),
            .dstStageMask = static_cast<PipelineStageFlags>(
                PipelineStageFlagBits::FragmentShader
            ),
            .srcAccessMask = {}, // you can set this to ColorAttachmentWrite or DepthStencilAttachmentWrite if needed
            .dstAccessMask = static_cast<AccessFlags>(
                AccessFlagBits::ShaderRead
            )
        },
        .useDepth = true, 
        };
  
    PC_CORE::RenderPassDescriptor renderPassDescriptor =
        {
            .colorAttachement = colorAttachement,
            .depthAttachment = &depthAttachement,
            .subPasses = subPassDescriptions
        };

    forwardPass = Rhi::CreateRenderPass(renderPassDescriptor);
    
}
