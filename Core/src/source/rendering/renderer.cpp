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
#include "rendering/render_system.hpp"
#include "resources/shader_source_binary.hpp"

using namespace PC_CORE;


void Renderer::GetRenderingData(const RenderingWorldData& _newRenderingData)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);


    m_RenderWorldData.Clear();
    m_RenderWorldData = _newRenderingData;
}

void Renderer::Init()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    m_RhiContext = Rhi::GetRhiContext();

    CreateRenderPasss();
    CreateShaders();
    CreateThirdPartyResources();
    CreateDescriptorSets();


    //m_DebugDrawContext = std::make_unique<DebugDrawContext>(this);
}

void Renderer::BeginDraw(Window* _window)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

	m_RhiContext->swapChain->GetSwapChainImageIndex(_window);
}

void Renderer::UpdateLightData(const RenderingContext& _context, CommandList* commandList)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    
    size_t updateDirLight = 0;
    size_t spotLight = 0;
    size_t pointLightUpdate = 0;

    for (size_t i = 0; i < m_RenderWorldData.lightData.size(); i++)
    {
        LightData& lightData = m_RenderWorldData.lightData[i];


        switch (lightData.lightType)
        {
        case LightType::Directional:

            if (updateDirLight >= MAX_DIRLIGHT)
                continue;
            {
                DirectionalLightGPU& gpudirLightData = gpuDynamicLightData->directionalLights[updateDirLight];
                gpudirLightData.color = lightData.data.directionalLight.color;
                gpudirLightData.direction = lightData.data.directionalLight.direction;
                gpudirLightData.intensity = lightData.data.directionalLight.intensity;
            }
            updateDirLight++;
            break;
        case LightType::Spotlight:
            if (spotLight >= MAX_SPOTLIGHT)
                continue;
            {
            SpotLightGPU& spotLightGPU = gpuDynamicLightData->spothLights[spotLight];
            spotLightGPU =
                {
                .position = static_cast<Tbx::Vector3f>(lightData.data.spotLight.position - _context.lowLevelCamera.position),
                .intensity = lightData.data.spotLight.intensity,
                .direction = lightData.data.spotLight.direction,
                .cutoff = lightData.data.spotLight.cutoff,
                .color = lightData.data.spotLight.color,
                .outerCutOff = lightData.data.spotLight.outerCutOff,
                .maxRange = std::sqrt(lightData.data.spotLight.intensity)
                };
            }
            spotLight++;
            break;
        case LightType::Point:
            if (pointLightUpdate >= MAX_SPOTLIGHT)
                continue;

        {
            PointLightGPU& pointLightGpu = gpuDynamicLightData->pointLights[pointLightUpdate];
            pointLightGpu.position = static_cast<Tbx::Vector3f>(lightData.data.pointLightData.position - _context.lowLevelCamera.position);
            pointLightGpu.maxRange = std::sqrt(lightData.data.pointLightData.intensity);
            pointLightGpu.intensity = lightData.data.pointLightData.intensity;
            pointLightGpu.color = lightData.data.pointLightData.color;
        }
            pointLightUpdate++;
            break;
        case LightType::Area:
        case LightType::Count:
        default:
            assert(false && "Not implemented light type");
        }
    }

    gpuLightUniformBufferStaging.Update(gpuDynamicLightData.get(), sizeof(GPUDynamicLightData));
    commandList->CopyBuffer(*gpuLightUniformBufferStaging.GetRhiBuffer(),
                               *gpuLightUniformBuffer.GetRhiBuffer(), 0, 0, sizeof(GPUDynamicLightData),
                               GpuPipelineStageFlagBits::FragmentShader);
}


void Renderer::UpdateCameraUniformBuffer(const PC_CORE::RenderingContext& renderingContext)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);


    currentRenderingContext = &renderingContext;
    SceneBufferGPU& sceneBufferGpu = sceneBufferGPU;

    sceneBufferGpu.time = PC_CORE::Time::GetTime();
    sceneBufferGPU.deltatime = PC_CORE::Time::DeltaTime();

    Tbx::Matrix4x4f view = Tbx::LookAtRH<float>(Tbx::Vector3f::Zero(),
                                                renderingContext.lowLevelCamera.front,
                                                renderingContext.lowLevelCamera.up);
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
    sceneBufferGPU.cameraPos = static_cast<Tbx::Vector3f>(renderingContext.lowLevelCamera.position);

    cameraUniformBuffer.Update(&sceneBufferGPU, sizeof(sceneBufferGPU));
}


void Renderer::DrawToRenderingContext(const PC_CORE::RenderingContext& renderingContext)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    primaryCommandList->Reset();
    primaryCommandList->BeginRecordCommands();
    //m_DebugDrawContext->Prepare();

    UpdateLightData(renderingContext, primaryCommandList.get());
    UpdateCameraUniformBuffer(renderingContext);

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


    DefferdPass(renderingContext, viewportInfo);
    ForwardPass(renderingContext, viewportInfo);
    FinalPass(renderingContext, viewportInfo);
}


void Renderer::SwapBuffers(Window* _window)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);


    std::shared_ptr<PC_CORE::SwapChain> swapChain = RhiContext::GetContext().swapChain;
    swapChain->BeginSwapChainRenderPass(primaryCommandList.get());

    primaryCommandList->ExucuteFetchCommand();
    swapChain->EndSwapChainRenderPass(primaryCommandList.get());

    primaryCommandList->EndRecordCommands();
    ClearRenderData();
    m_RhiContext->swapChain->Present(primaryCommandList.get(), _window);
    Rhi::NextFrame();
}


void Renderer::DrawStaticMesh(MaterialType type, std::shared_ptr<PC_CORE::GraphicShader> shader)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    Tbx::Vector3d cameraOffset = static_cast<Tbx::Vector3d>(currentRenderingContext->lowLevelCamera.position);
    for (size_t i = 0; i < m_RenderWorldData.staticMeshData.size(); i++)
    {
        if (!m_RenderWorldData.staticMeshData[i].mesh->IsLoaded())
            continue;

        if (m_RenderWorldData.staticMeshData[i].materialType != type)
            continue;

        Tbx::Matrix4x4f modelMatrixf[2];
        // Apply offset to the copy one
        modelMatrixf[0] = m_RenderWorldData.staticMeshData[i].worldMatrix;
        modelMatrixf[0][12] -= cameraOffset.x;
        modelMatrixf[0][13] -= cameraOffset.y;
        modelMatrixf[0][14] -= cameraOffset.z;

        modelMatrixf[1] = m_RenderWorldData.staticMeshData[i].normalInvertMatrix;


        const ShaderProgramDescriptorSets* materialDescriptor = m_RenderWorldData.staticMeshData[i].descriptorSet;
        const Mesh* mesh = m_RenderWorldData.staticMeshData[i].mesh;

        // Send Data

        primaryCommandList->BindDescriptorSet(shader.get(), materialDescriptor, MATERIAL_DESCRIPTOR_SET, 1);

        primaryCommandList->PushConstant(shader.get(), "PushConstants", &modelMatrixf,
                                         sizeof(Tbx::Matrix4x4f) * 2);
        primaryCommandList->BindVertexBuffer(*mesh->vertexBuffer.GetRhiBuffer(), 0, 1);
        primaryCommandList->BindIndexBuffer(*mesh->indexBuffer.GetRhiBuffer(), 0);
        primaryCommandList->DrawIndexed(mesh->indexBuffer.GetIndexCount(), 1, 0, 0, 0);
    }
}

void Renderer::ClearRenderData()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    m_RenderWorldData.lightData.clear();
    m_RenderWorldData.staticMeshData.clear();
}


void Renderer::DrawSkyBox()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    if (auto cube = m_CubeMesh.lock())
    {
        primaryCommandList->BindProgram(m_CubeMapShader.lock().get());
        primaryCommandList->BindDescriptorSet(m_CubeMapShader.lock().get(), descriptorSetsSkybox.cameraDescriptorSet,
                                              SCENE_DESCRIPTOR_SET, 1);
        primaryCommandList->BindDescriptorSet(m_CubeMapShader.lock().get(), descriptorSetsSkybox.cubeMapDescriptorSet,
                                              ENVIRONEMENT_DESCRIPTOR_SET, 1);
        primaryCommandList->BindVertexBuffer(*cube->vertexBuffer.GetRhiBuffer(), 0, 1);
        primaryCommandList->BindIndexBuffer(*cube->indexBuffer.GetRhiBuffer(), 0);
        primaryCommandList->DrawIndexed(cube->indexBuffer.GetIndexCount(), 1, 0, 0, 0);
    }
}


void Renderer::ForwardPass(const PC_CORE::RenderingContext& _renderingContext, const ViewportInfo& _viewportInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    const BeginRenderPassInfo beginRenderPassInfo =
    {
        .renderPass = renderPasses.forwardPass,
        .frameBuffer = _renderingContext.forwardFrameBuffer,
        .renderOffSet = {0, 0},
        .extent = {_renderingContext.renderingContextSize.x, _renderingContext.renderingContextSize.y},
        .clearValueFlags = {},
        .clearColor = nullptr,
        .clearValueCount = 0,
        .clearDepth = 1.f
    };


    primaryCommandList->BeginDebugLabel("Forward Pass", FORWARD_DEBUG_COLOR);
    primaryCommandList->BeginRenderPass(beginRenderPassInfo);

    primaryCommandList->BindProgram(m_ForwardShader.lock().get());
    primaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleList);

    primaryCommandList->SetViewPort(_viewportInfo);
    primaryCommandList->BindDescriptorSet(m_ForwardShader.lock().get(), m_ShaderProgramSceneDescriptorSet,
                                          SCENE_DESCRIPTOR_SET, 1);

    // draw all static mesh
    DrawStaticMesh(MaterialType::Transparent, m_ForwardShader.lock());
    DrawSkyBox();
#ifdef WITH_EDITOR
    for (auto& it : UserCustomForwardPass)
        it(*this, primaryCommandList.get(), *currentRenderingContext, &m_RenderWorldData);
    //m_DebugDrawContext->DrawDebugPrimitive(primaryCommandList.get(), _renderingContext);
#endif
    primaryCommandList->EndRenderPass();

    primaryCommandList->EndDebugLabel();
}

void Renderer::DefferdPass(const PC_CORE::RenderingContext& _renderingContext, const ViewportInfo& _viewportInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    ClearValueFlags clearValueFlags = static_cast<ClearValueFlags>(ClearValueFlags::ClearValueColor |
        ClearValueFlags::ClearValueDepth);

    std::array<Tbx::Vector4f, static_cast<uint8_t>(GbufferType::Count)> clearValues2 = {
        Tbx::Vector4f(0, 1, 0, 1.f),
    };

    const BeginRenderPassInfo beginRenderPassInfo =
    {
        .renderPass = renderPasses.defferedPass,
        .frameBuffer = _renderingContext.gbufferFrameBuffer,
        .renderOffSet = {0, 0},
        .extent = {_renderingContext.renderingContextSize.x, _renderingContext.renderingContextSize.y},
        .clearValueFlags = clearValueFlags,
        .clearColor = clearValues2.data(),
        .clearValueCount = clearValues2.size(),
        .clearDepth = 1.f
    };

    primaryCommandList->BeginDebugLabel("Gbuffer Pass", GEOMETRY_PASS_COLOR);
    primaryCommandList->BeginRenderPass(beginRenderPassInfo);

    if (std::shared_ptr sGeometry = m_GeometryBufferShader.lock())
    {
        primaryCommandList->BindProgram(sGeometry.get());
        primaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleList);

        primaryCommandList->BindDescriptorSet(sGeometry.get(), m_GeometryBufferDescriptorSet, SCENE_DESCRIPTOR_SET, 1);
        DrawStaticMesh(MaterialType::Opaque, sGeometry);
    }
    primaryCommandList->EndDebugLabel();

    primaryCommandList->NextSubPass();

    primaryCommandList->BeginDebugLabel("DeferredPass", DEFERD_PASS_COLOR);

    if (std::shared_ptr sDeferred = m_DeferedShader.lock())
    {
        primaryCommandList->BindProgram(sDeferred.get());
        primaryCommandList->BindDescriptorSet(sDeferred.get(), m_DeferdDescriptorSet, SCENE_DESCRIPTOR_SET, 1);
        primaryCommandList->BindDescriptorSet(sDeferred.get(), _renderingContext.gbufferDescriptorSet, GBUFFER_SET, 1);

        primaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleStrip);
        primaryCommandList->Draw(4, 1, 0, 0);
    }

    primaryCommandList->EndDebugLabel();

    primaryCommandList->EndRenderPass();
}


void Renderer::FinalPass(const PC_CORE::RenderingContext& _renderingContext, const ViewportInfo& _viewportInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    std::array<Tbx::Vector4f, 2> clearValues2 = {
        Tbx::Vector4f(0, 0, 0, 0.f),
        Tbx::Vector4f(0, 0, 0, 0.f),
    };

    const BeginRenderPassInfo drawToViewport =
    {
        .renderPass = renderPasses.drawToFinalViewPort,
        .frameBuffer = _renderingContext.finalImageFrameBuffer,
        .renderOffSet = {0, 0},
        .extent = {_renderingContext.renderingContextSize.x, _renderingContext.renderingContextSize.y},
        .clearValueFlags = static_cast<ClearValueFlags>(ClearValueFlags::ClearValueColor),
        .clearColor = clearValues2.data(),
        .clearValueCount = clearValues2.size(),
        .clearDepth = 0.f,
        .clearStencil = 0.f
    };


    primaryCommandList->BeginDebugLabel("Final Pass", FINAL_RENDER_PASS_DEBUG_COLOR);
    primaryCommandList->BeginRenderPass(drawToViewport);
    primaryCommandList->BindProgram(m_DrawTextureScreenQuadShader.lock().get());
    primaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleStrip);

    primaryCommandList->BindDescriptorSet(m_DrawTextureScreenQuadShader.lock().get(),
                                          _renderingContext.viewPortDescriptorSet, 0, 1);
    primaryCommandList->Draw(4, 1, 0, 0);

    primaryCommandList->EndRenderPass();
    primaryCommandList->EndDebugLabel();
}


#pragma region CreateRenderPasss

void Renderer::CreateRenderPasss()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    {
        PERF_REGION_SCOPED_NAMED("Create Defferd RenderPass");

        std::vector<RenderPassAttachementDescriptor> attachements;
        // + 1 final image 
        attachements.resize(
            static_cast<std::vector<RenderPassAttachementDescriptor>::size_type>(GbufferType::Depth) + 1);

        attachements[static_cast<uint8_t>(GbufferType::Albedo)] =
        {
            .attachmentType = AttachmentType::Color,
            .format = PC_CORE::RHIFormat::R8G8B8A8_UNORM,
            .sampleCount = 1,
            .load = LoadOperation::Clear,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
        };
        attachements[static_cast<uint8_t>(GbufferType::Normal)] =
        {
            .attachmentType = AttachmentType::Color,
            .format = PC_CORE::RHIFormat::R16G16_SNORM,
            .sampleCount = 1,
            .load = LoadOperation::Clear,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
        };
        attachements[static_cast<uint8_t>(GbufferType::RoughnessMetallicAo)] =
        {
            .attachmentType = AttachmentType::Color,
            .format = PC_CORE::RHIFormat::R8G8B8A8_UNORM,
            .sampleCount = 1,
            .load = LoadOperation::Clear,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
        };
        attachements[static_cast<uint8_t>(GbufferType::WorldPosition)] =
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

        // out image
        attachements[attachements.size() - 1] =
        {
            .attachmentType = AttachmentType::Color,
            .format = PC_CORE::RHIFormat::R8G8B8A8_UNORM,
            .sampleCount = 1,
            .load = LoadOperation::Clear,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
        };

        // Geometry subpass and deffered lighting
        std::vector<SubPassDescription> subPassDescriptions;
        subPassDescriptions.resize(2);

        subPassDescriptions[0] =
        {
            .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
            .colorAttachementDescriptorIndicies = {
                static_cast<size_t>(GbufferType::Albedo),
                static_cast<size_t>(GbufferType::Normal),
                static_cast<size_t>(GbufferType::RoughnessMetallicAo),
                static_cast<size_t>(GbufferType::WorldPosition)
            },
            .inputAttachementDescriptorIndicies = {},
            .subPassDependcies =
            {
                .srcStageMask = GpuPipelineStageFlagBits::ColorAttachmentOutput |
                GpuPipelineStageFlagBits::EarlyFragmentTests,
                .dstStageMask = GpuPipelineStageFlagBits::FragmentShader,
                .srcAccessMask = {},
                .dstAccessMask = GpuAccessFlag::ShaderRead

            },
            .useDepth = true,
        };
        subPassDescriptions[1] =
        {
            .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
            .colorAttachementDescriptorIndicies =
            {
                attachements.size() - 1
            },
            .inputAttachementDescriptorIndicies = {
                static_cast<size_t>(GbufferType::Albedo),
                static_cast<size_t>(GbufferType::Normal),
                static_cast<size_t>(GbufferType::RoughnessMetallicAo),
                static_cast<size_t>(GbufferType::WorldPosition)
            },
            .subPassDependcies =
            {
                .srcStageMask =
                GpuPipelineStageFlagBits::ColorAttachmentOutput,
                .dstStageMask =
                GpuPipelineStageFlagBits::FragmentShader,
                .srcAccessMask = {},
                .dstAccessMask = GpuAccessFlag::ShaderRead
            },
            .useDepth = false,
        };
        PC_CORE::RenderPassDescriptor renderPassDescriptor =
        {
            .attachement = attachements,
            .depthAttachment = &depthAttachement,
            .subPasses = subPassDescriptions
        };

        renderPasses.defferedPass = Rhi::CreateRenderPass(renderPassDescriptor);
    }

    // Forward
    {
        PERF_REGION_SCOPED_NAMED("Create Forward RenderPass");

        std::vector<RenderPassAttachementDescriptor> colorAttachement;
        colorAttachement.resize(1);

        colorAttachement[0] =
        {
            .attachmentType = AttachmentType::Color,
            .format = PC_CORE::RHIFormat::R8G8B8A8_UNORM,
            .sampleCount = 1,
            .load = LoadOperation::Load,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
        };

        RenderPassAttachementDescriptor depthAttachement =
        {
            .attachmentType = AttachmentType::Depth,
            .format = PC_CORE::RHIFormat::D32_SFLOAT,
            .sampleCount = 1,
            .load = LoadOperation::Load,
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
            .inputAttachementDescriptorIndicies = {},
            .subPassDependcies =
            {
                .srcStageMask = GpuPipelineStageFlagBits::ColorAttachmentOutput |
                GpuPipelineStageFlagBits::EarlyFragmentTests,
                .dstStageMask = GpuPipelineStageFlagBits::FragmentShader,
                .srcAccessMask = {},
                // you can set this to ColorAttachmentWrite or DepthStencilAttachmentWrite if needed
                .dstAccessMask = GpuAccessFlag::ShaderRead
            },
            .useDepth = true,
        };

        PC_CORE::RenderPassDescriptor renderPassDescriptor =
        {
            .attachement = colorAttachement,
            .depthAttachment = &depthAttachement,
            .subPasses = subPassDescriptions
        };

        renderPasses.forwardPass = Rhi::CreateRenderPass(renderPassDescriptor);
    }

    // Draw To Final Viewport
    {
        PERF_REGION_SCOPED_NAMED("Create Draw To Final Viewport");
        renderPasses.drawToFinalViewPort = Rhi::CreateRenderPass(PC_CORE::RHIFormat::R8G8B8A8_UNORM,
                                                                 Rhi::GetRhiContext()->physicalDevices->
                                                                 GetPhysicalDevice().GetMaxUsableSampleCount());
    }
}


#pragma endregion CreateRenderPasss

#pragma region CreateShaders
void Renderer::CreateShaders()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);


    {
        PERF_REGION_SCOPED_NAMED("Geometry Shader");
        constexpr RasterizerInfo rasterizerInfo =
        {
            .polygonMode = PolygonMode::Fill,
            .cullModeFlag = CullModeFlagBit::Back,
            .frontFace = FrontFace::CounterClockwise
        };


        const ShaderGraphicPointInfo shaderGraphicPointInfo =
        {
            .rasterizerInfo = rasterizerInfo,
            .dephInfo =
            {
                .depthCompareOp = CompareOp::LESS,
                .enableDepthTest = true
            },
            .vertexInputBindingDescritions = {Vertex::GetBindingDescrition(0)},
            .vertexAttributeDescriptions = Vertex::GetAttributeDescriptions(0),
        };

        const SourceList sources =
        {
            {
                ShaderStageTypeFlag::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("geometry_spv.vert"),
            },
            {
                ShaderStageTypeFlag::Fragment,
                ResourceManager::Get<ShaderSourceBinary>("geometry_spv.frag")
            }
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
            .shaderGraphicPointInfo = shaderGraphicPointInfo,
            .sourceList = sources,
            .renderPass = renderPasses.defferedPass.get(),
            // COUNT because we avoid depth but there is still final image 
            .colorAttachementCount = 4,
            .subPassIndex = 0,
        };

        m_GeometryBufferShader = ResourceManager::Create<GraphicShader>("Geometry", graphicShaderProgramCreateInfo);
    }

    {
        PERF_REGION_SCOPED_NAMED("Defferd Shader");
        constexpr RasterizerInfo rasterizerInfo =
        {
            .polygonMode = PolygonMode::Fill,
            .cullModeFlag = CullModeFlagBit::None,
            .frontFace = FrontFace::CounterClockwise
        };


        const ShaderGraphicPointInfo shaderGraphicPointInfo =
        {
            .rasterizerInfo = rasterizerInfo,
            .dephInfo =
            {
                .depthCompareOp = CompareOp::NEVER,
                .enableDepthTest = true
            },
            .vertexInputBindingDescritions = {},
            .vertexAttributeDescriptions = {},
        };

        const SourceList sources =
        {
            {
                ShaderStageTypeFlag::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("draw_texture_screen_quad_spv.vert"),
            },
            {
                ShaderStageTypeFlag::Fragment,
                ResourceManager::Get<ShaderSourceBinary>("deferred_spv.frag")
            }
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
            .shaderGraphicPointInfo = shaderGraphicPointInfo,
            .sourceList = sources,
            .renderPass = renderPasses.defferedPass.get(),
            .colorAttachementCount = 1,
            .subPassIndex = 1,
        };

        m_DeferedShader = ResourceManager::Create<GraphicShader>("Deferred", graphicShaderProgramCreateInfo);
    }

    {
        PERF_REGION_SCOPED_NAMED("Forward Shader");
        constexpr RasterizerInfo rasterizerInfo =
        {
            .polygonMode = PolygonMode::Fill,
            .cullModeFlag = CullModeFlagBit::Back,
            .frontFace = FrontFace::CounterClockwise
        };


        const ShaderGraphicPointInfo shaderGraphicPointInfo =
        {
            .rasterizerInfo = rasterizerInfo,
            .dephInfo =
            {
                .depthCompareOp = CompareOp::LESS,
                .enableDepthTest = true
            },
            .vertexInputBindingDescritions = {Vertex::GetBindingDescrition(0)},
            .vertexAttributeDescriptions = Vertex::GetAttributeDescriptions(0),
        };

        const SourceList sources =
        {
            {
                ShaderStageTypeFlag::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("forward_spv.vert"),
            },
            {
                ShaderStageTypeFlag::Fragment,
                ResourceManager::Get<ShaderSourceBinary>("forward_spv.frag")
            }
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
            .shaderGraphicPointInfo = shaderGraphicPointInfo,
            .sourceList = sources,
            .renderPass = renderPasses.forwardPass.get(),
            .colorAttachementCount = 1,
            .subPassIndex = 0
        };

        m_ForwardShader = ResourceManager::Create<GraphicShader>("ForwardShader", graphicShaderProgramCreateInfo);
    }

    // SkyBox Shader
    {
        PERF_REGION_SCOPED_NAMED("SkyBox Shader");

        const RasterizerInfo rasterizerInfo =
        {
            .polygonMode = PolygonMode::Fill,
            .cullModeFlag = CullModeFlagBit::None,
            .frontFace = FrontFace::CounterClockwise,
            .multiSampleRasterization = 1
        };

        VertexAttributeDescription vertexAttributeDescription =
        {
            .binding = 0,
            .location = 0,
            .format = RHIFormat::R32G32B32_SFLOAT,
            .offset = 0
        };

        const ShaderGraphicPointInfo shaderGraphicPointInfo =
        {
            .rasterizerInfo = rasterizerInfo,
            .dephInfo =
            {
                .depthCompareOp = CompareOp::LESS_OR_EQUAL,
                .enableDepthTest = true
            },
            .vertexInputBindingDescritions = {Vertex::GetBindingDescrition(0)},
            .vertexAttributeDescriptions = {Vertex::GetAttributeDescriptions(0)},
        };

        const SourceList source =
        {
            {
                ShaderStageTypeFlag::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("cube_map_skybox_spv.vert")
            },
            {
                ShaderStageTypeFlag::Fragment,
                ResourceManager::Get<ShaderSourceBinary>("cube_map_skybox_spv.frag")
            }
        };

        const ShaderInfo shaderInfo =
        {
            .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
            .shaderInfoData = shaderGraphicPointInfo,
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
            .shaderGraphicPointInfo = shaderGraphicPointInfo,
            .sourceList = source,
            .renderPass = renderPasses.forwardPass.get(),
            .colorAttachementCount = 1,
            .subPassIndex = 0

        };


        m_CubeMapShader = ResourceManager::Create<PC_CORE::GraphicShader>(
            "SkyboxShader", graphicShaderProgramCreateInfo);
    }

    // Draw to final viewport
    {
        PERF_REGION_SCOPED_NAMED("CreateDrawToFinalViewport Programm");

        const RasterizerInfo rasterizerInfo =
        {
            .polygonMode = PolygonMode::Fill,
            .cullModeFlag = CullModeFlagBit::None,
            .frontFace = FrontFace::CounterClockwise,
            .multiSampleRasterization = Rhi::GetRhiContext()->physicalDevices->GetPhysicalDevice().
                                                              GetMaxUsableSampleCount()
        };


        const ShaderGraphicPointInfo shaderGraphicPointInfo =
        {
            .rasterizerInfo = rasterizerInfo,
            .dephInfo =
            {
                .depthCompareOp = CompareOp::LESS,
                .enableDepthTest = true
            },
            .vertexInputBindingDescritions = {},
            .vertexAttributeDescriptions = {},
        };

        const SourceList sources =
        {
            {
                ShaderStageTypeFlag::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("draw_texture_screen_quad_spv.vert"),
            },
            {
                ShaderStageTypeFlag::Fragment,
                ResourceManager::Get<ShaderSourceBinary>("draw_texture_screen_quad_spv.frag")
            }
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
            .shaderGraphicPointInfo = shaderGraphicPointInfo,
            .sourceList = sources,
            .renderPass = renderPasses.drawToFinalViewPort.get(),
            .colorAttachementCount = 1,

        };


        m_DrawTextureScreenQuadShader = ResourceManager::Create<GraphicShader>(
            "DrawQuadShader", graphicShaderProgramCreateInfo);
    }
}

#pragma endregion CreateShaders


#pragma region CreateThirdPartyResources

void Renderer::CreateThirdPartyResources()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    {
        PERF_REGION_SCOPED_NAMED("Create Cube Map");
        std::array<std::string, 6> maps
        {
            "assets/textures/skybox/right.jpg",
            "assets/textures/skybox/left.jpg",
            "assets/textures/skybox/top.jpg",
            "assets/textures/skybox/bottom.jpg",
            "assets/textures/skybox/front.jpg",
            "assets/textures/skybox/back.jpg",
        };
        m_Cubemap = ResourceManager::Create<Texture3D>("BasicCubemap", maps);
    }
    {
        gpuDynamicLightData = std::make_unique<GPUDynamicLightData>();

        gpuLightUniformBufferStaging = UniformBuffer(&gpuDynamicLightData, sizeof(GPUDynamicLightData),
                                                     PC_CORE::MemoryLocalisation::CPU_To_GPU, MemoryUsage::Dynamic);
        gpuLightUniformBuffer = UniformBuffer(&gpuDynamicLightData, sizeof(GPUDynamicLightData),
                                              PC_CORE::MemoryLocalisation::GPU_Only, MemoryUsage::Dynamic);

        cameraUniformBuffer = UniformBuffer(&sceneBufferGPU, sizeof(sceneBufferGPU),
                                            PC_CORE::MemoryLocalisation::CPU_To_GPU, MemoryUsage::Dynamic);

        m_CubeMesh = ResourceManager::Get<Mesh>("cube.obj");
    }
}
#pragma endregion CreateThirdPartyResources

#pragma region CreateDescriptorSets

void Renderer::CreateDescriptorSets()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    /////////////////////////////////////////////////
    UniformBufferDescriptor cameraBufferDescritptor
    {
        .buffer = &cameraUniformBuffer,
    };

    UniformBufferDescriptor lightData
    {
        .buffer = &gpuLightUniformBuffer,
    };

    ImageSamperDescriptor skyboxCubeMapDescritptor
    {
        .sampler = ResourceManager::Get<Sampler>("LinearRepeat").get(),
        .texture = m_Cubemap.lock().get()
    };

    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets;

    {
        PERF_REGION_SCOPED_NAMED("Create Geometry Shader DescriptorSet");
        descriptorSets =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                &cameraBufferDescritptor,
                nullptr,
            }
        };
        m_GeometryBufferShader.lock()->AllocDescriptorSet(&m_GeometryBufferDescriptorSet, SCENE_DESCRIPTOR_SET);
        m_GeometryBufferDescriptorSet->WriteDescriptorSets(descriptorSets);
    }

    {
        PERF_REGION_SCOPED_NAMED("Create Deferred Shader DescriptorSet");
        descriptorSets =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                &cameraBufferDescritptor,
                nullptr,
            },
            {
                ShaderProgramDescriptorType::UniformBuffer,
                LIGHTDATA_BINDING,
                &lightData,
                nullptr,
            }
        };
        m_DeferedShader.lock()->AllocDescriptorSet(&m_DeferdDescriptorSet, SCENE_DESCRIPTOR_SET);
        m_DeferdDescriptorSet->WriteDescriptorSets(descriptorSets);
    }

    {
        PERF_REGION_SCOPED_NAMED("Create Forward Shader DescriptorSet");

        descriptorSets =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                &cameraBufferDescritptor,
                nullptr,
                nullptr
            },
            {
                ShaderProgramDescriptorType::UniformBuffer,
                LIGHTDATA_BINDING,
                &lightData,
                nullptr,
                nullptr
            },
            {
                ShaderProgramDescriptorType::CombinedImageSampler,
                FORWARD_SKYBOX_CUBEMAP,
                nullptr,
                &skyboxCubeMapDescritptor,
                nullptr
            }
        };

        m_ForwardShader.lock()->AllocDescriptorSet(&m_ShaderProgramSceneDescriptorSet, SCENE_DESCRIPTOR_SET);
        m_ShaderProgramSceneDescriptorSet->WriteDescriptorSets(descriptorSets);
    }


    {
        PERF_REGION_SCOPED_NAMED("Skybox Shader DescriptorSets");

        descriptorSets =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                &cameraBufferDescritptor,
                nullptr,
            },
        };
        m_CubeMapShader.lock()->AllocDescriptorSet(&descriptorSetsSkybox.cameraDescriptorSet, SCENE_DESCRIPTOR_SET);
        descriptorSetsSkybox.cameraDescriptorSet->WriteDescriptorSets(descriptorSets);

        descriptorSets =
        {
            {
                ShaderProgramDescriptorType::CombinedImageSampler,
                SKYBOX_BINDING,
                nullptr,
                &skyboxCubeMapDescritptor,
            }
        };
        m_CubeMapShader.lock()->AllocDescriptorSet(&descriptorSetsSkybox.cubeMapDescriptorSet,
                                                   ENVIRONEMENT_DESCRIPTOR_SET);
        descriptorSetsSkybox.cubeMapDescriptorSet->WriteDescriptorSets(descriptorSets);
    }
}
#pragma endregion CreateDescriptorSets
