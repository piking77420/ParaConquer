#include "Rendering/Renderer.hpp"

#include <Thread>
#include <PerfRegion.hpp>

#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/Vertex.hpp"
#include "Resources/ResourceManager.hpp"
#include "Time/CoreTime.hpp"
#include "Math/MatrixTransformation.hpp"
#include "World/StaticMeshComponent.hpp"
#include "World/Transform.hpp"
#include "Rendering/RenderPasses/RenderPass.hpp"


#include "Math/ToolboxTypedef.hpp"
#include "Rendering/RenderSystem.hpp"
#include "Resources/ShaderSourceBinary.hpp"

#include "VulkanCommandList.hpp"
#include "Resources/ComputeShader.hpp"

using namespace PC_CORE;


void Renderer::GetRenderingData(const RenderingWorldData& _newRenderingData)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);


    RenderWorldData.Clear();
    RenderWorldData = _newRenderingData;
}

Renderer::~Renderer()
{
    for (auto& it : m_Views)
    {
        if (it.use_count() != 1)
        {
            PC_LOGERROR("There is still reference to a view")
        }
    }
}

void Renderer::Init()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    m_RhiContext = Rhi::GetRhiContext();

    CommandListCreateInfo commandListCreateInfo =
    {
        .CommandPoolFamily = CommandPoolFamily::Graphics,
        .CommandBufferType = CommandBufferType::Primary,
    };

    PrimaryCommandList = Rhi::CreateCommandList(commandListCreateInfo);
    SwapChainPassCommandList = Rhi::CreateCommandList(commandListCreateInfo);

    const SamplerCreateInfo info =
    {
        .SamplerName = "LinearReapeat",
        .magFilter = Filter::Linear,
        .minFilter = Filter::Linear,
        .u = SamplerAddressMode::Repeat,
        .v = SamplerAddressMode::Repeat,
        .w = SamplerAddressMode::Repeat
    };

    LinearReapeat = Sampler(info);

    const SamplerCreateInfo skyBoxSampler =
    {
        .SamplerName = "SkyBoxSampler",
        .magFilter = Filter::Linear,
        .minFilter = Filter::Linear,
        .u = SamplerAddressMode::ClampToEdge,
        .v = SamplerAddressMode::ClampToEdge,
        .w = SamplerAddressMode::ClampToEdge
    };

    m_SkyBoxSampler = Sampler(skyBoxSampler);


    InitCubeBuffers();
    CreateRenderPasss();
    CreateShaders();
    CreateBuffers();
    CreateThirdPartyResources();
    CreateDescriptorSets();
#ifdef WITH_EDITOR
    m_DebugDrawContext = std::make_unique<DebugDrawContext>(this);
#endif
}

void Renderer::BeginFrame(Window* _window)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    m_RhiContext->swapChain->GetSwapChainImageIndex(_window);
}

void Renderer::UpdateGpuCameraData()
{
    const auto& rContextView = m_CurrentView->RenderingContext;
    const auto& gpuCamera = m_CurrentView->CameraGpu;


    UniformBuffers.CameraUniformBuffer.Update(&gpuCamera, sizeof(CameraGpu));
}

void Renderer::UpdateLightGpuData(CommandList* _commandlist)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    const auto& rContextView = m_CurrentView->RenderingContext;
    const auto& gpuCamera = m_CurrentView->CameraGpu;

    int updateDirLight = 0;
    int spotLight = 0;
    int pointLightUpdate = 0;

    for (size_t i = 0; i < RenderWorldData.LightData.size(); i++)
    {
        LightData& lightData = RenderWorldData.LightData[i];

        switch (lightData.LightType)
        {
        case LightType::Directional:

            if (updateDirLight >= MAX_DIRLIGHT)
                continue;
            {
                DirectionalLightGpu& gpudirLightData = m_GpuDynamicLightData.DirectionalLights[updateDirLight];
                gpudirLightData.Color = lightData.Data.DirectionalLight.Color;
                gpudirLightData.Direction = lightData.Data.DirectionalLight.Direction;
                gpudirLightData.Intensity = lightData.Data.DirectionalLight.Intensity;
            }
            updateDirLight++;
            break;
        case LightType::Spotlight:
            if (spotLight >= MAX_SPOTLIGHT)
                continue;
            {
                SpotLightGpu& spotLightGPU = m_GpuDynamicLightData.SpothLights[spotLight];
                spotLightGPU =
                {
                    .Position = static_cast<Tbx::Vector3f>(lightData.Data.SpotLight.Position - rContextView.
                        LowLevelCamera.Position),
                    .Intensity = lightData.Data.SpotLight.Intensity,
                    .Direction = lightData.Data.SpotLight.Direction,
                    .Cutoff = lightData.Data.SpotLight.Cutoff,
                    .Color = lightData.Data.SpotLight.Color,
                    .OuterCutOff = lightData.Data.SpotLight.OuterCutOff,
                    .MaxRange = std::sqrt(lightData.Data.SpotLight.Intensity)
                };
            }
            spotLight++;
            break;
        case LightType::Point:
            if (pointLightUpdate >= MAX_POINTLIGHT)
                continue;

            {
                const auto p3 = static_cast<Tbx::Vector3f>(lightData.Data.PointLightData.Position - rContextView.
                    LowLevelCamera.Position);
                const Tbx::Vector4f p4 = gpuCamera.View * Tbx::Vector4f(p3.x, p3.y, p3.z, 1.0);
                const auto p3ViewSpace = Tbx::Vector3f(p4.x, p4.y, p4.z);

                PointLightGpu& pointLightGpu = m_GpuDynamicLightData.PointLights[pointLightUpdate];
                pointLightGpu.Position = p3ViewSpace;
                pointLightGpu.MaxRange = std::sqrt(lightData.Data.PointLightData.Intensity);
                pointLightGpu.Intensity = lightData.Data.PointLightData.Intensity;
                pointLightGpu.Color = lightData.Data.PointLightData.Color;
            }
            pointLightUpdate++;
            break;
        case LightType::Area:
        case LightType::Count:
        default:
            assert(false && "Not implemented light type");
        }
    }
    m_GpuDynamicLightData.DirLightCount = updateDirLight;
    m_GpuDynamicLightData.SpothLightCount = spotLight;
    m_GpuDynamicLightData.PointLightCount = pointLightUpdate;


    constexpr size_t size = sizeof(GPUDynamicLightData);
    m_GpuLightUniformBufferStaging.Update(&m_GpuDynamicLightData, sizeof(m_GpuDynamicLightData));


    _commandlist->CopyBuffer(*m_GpuLightUniformBufferStaging.GetRhiBuffer(),
                            *UniformBuffers.DynamicGpuLightUniformBuffer.GetRhiBuffer(), 0, 0,
                            sizeof(GPUDynamicLightData));

    const BufferMemoryBarrier barrier =
    {
        .SrcAccessMask = GpuAccessFlag::TransferWrite,
        .DstAccessMask = GpuAccessFlag::ShaderRead,
        .Buffer = UniformBuffers.DynamicGpuLightUniformBuffer.GetRhiBuffer().get(),
        .Offset = 0,
        .Size = sizeof(GPUDynamicLightData),
    };
    _commandlist->Barrier(
        GpuPipelineStageFlagBits::Transfer,
        GpuPipelineStageFlagBits::FragmentShader,
        nullptr, 0,
        &barrier, 1,
        nullptr, 0);
}


void Renderer::Draw(const View& _view)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    

    m_CurrentView = &_view;
    const auto& rContextView = m_CurrentView->RenderingContext;

    UpdateGpuCameraData();
    
    PrimaryCommandList->Reset();
    PrimaryCommandList->BeginRecordCommands();
#ifdef WITH_EDITOR
    m_DebugDrawContext->Prepare();
#endif
    UpdateLightGpuData(PrimaryCommandList.get());

    const ViewportInfo viewportInfo =
    {
        .Transform = {0, 0},
        .Size = {
            static_cast<float>(rContextView.RenderingContextSize.x),
            static_cast<float>(rContextView.RenderingContextSize.y)
        },
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
        .ScissorsOff = {0, 0},
        .Scissorsextent = {rContextView.RenderingContextSize.x, rContextView.RenderingContextSize.y}
    };

    PrimaryCommandList->SetViewPort(viewportInfo);
    DefferdPass(viewportInfo);
    ForwardPass(viewportInfo);
    PostProcess(viewportInfo);
    FinalPass(viewportInfo);

    PrimaryCommandList->EndRecordCommands();
    PrimaryCommandList->Flush(FlushCommandMethod::Sync
                              , GpuPipelineStageFlagBits::ColorAttachmentOutput); // flush
}


void Renderer::SwapBuffers(Window* _window)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    std::shared_ptr<SwapChain> swapChain = RhiContext::GetContext().swapChain;


    SwapChainPassCommandList->Reset();
    SwapChainPassCommandList->BeginRecordCommands();

    swapChain->BeginSwapChainRenderPass(SwapChainPassCommandList.get());
    SwapChainPassCommandList->ExecuteExternalCommand();
    swapChain->EndSwapChainRenderPass(SwapChainPassCommandList.get());

    SwapChainPassCommandList->EndRecordCommands();
    SwapChainPassCommandList->Flush(FlushCommandMethod::Sync, GpuPipelineStageFlagBits::ColorAttachmentOutput);


    ClearRenderData();
    m_RhiContext->swapChain->Present(_window);
    Rhi::NextFrame();
}

std::shared_ptr<View> Renderer::CreateView(Tbx::Vector2i _defaultSize)
{
    auto view = std::make_shared<View>(this, _defaultSize);

    m_Views.push_back(view);

    return view;
}


void Renderer::DrawStaticMesh(MaterialType _type, const ObjectPtr<GraphicShader>& _shader)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    const auto& rContextView = m_CurrentView->RenderingContext;

    auto cameraOffset = rContextView.LowLevelCamera.Position;

    for (size_t i = 0; i < RenderWorldData.StaticMeshComponentData.size(); i++)
    {
        if (RenderWorldData.StaticMeshComponentData[i].MaterialType != _type)
            continue;

        Tbx::Matrix4x4f modelMatrixf[2];
        // Apply offset to the copy one
        modelMatrixf[0] = RenderWorldData.StaticMeshComponentData[i].WorldMatrix;
        modelMatrixf[0][12] -= cameraOffset.x;
        modelMatrixf[0][13] -= cameraOffset.y;
        modelMatrixf[0][14] -= cameraOffset.z;

        modelMatrixf[1] = RenderWorldData.StaticMeshComponentData[i].NormalInvertMatrix;


        const ShaderProgramDescriptorSets* materialDescriptor = RenderWorldData.StaticMeshComponentData[i].
            DescriptorSet;
        const StaticMesh* mesh = RenderWorldData.StaticMeshComponentData[i].StaticMesh;

        // Send Data

        PrimaryCommandList->BindDescriptorSet(_shader.get(), materialDescriptor, MATERIAL_DESCRIPTOR_SET, 1);

        PrimaryCommandList->PushConstant(_shader.get(), "pushConstant", &modelMatrixf,
                                         sizeof(Tbx::Matrix4x4f) * 2);
        PrimaryCommandList->BindVertexBuffer(*mesh->VBuffer.GetRhiBuffer(), 0, 1);
        PrimaryCommandList->BindIndexBuffer(*mesh->IBuffer.GetRhiBuffer(), 0);
        PrimaryCommandList->DrawIndexed(mesh->IBuffer.GetIndexCount(), 1, 0, 0, 0);
    }
}

void Renderer::ClearRenderData()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    RenderWorldData.LightData.clear();
    RenderWorldData.StaticMeshComponentData.clear();
}

void Renderer::ForwardPass(const ViewportInfo& _viewportInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    const auto& rContextView = m_CurrentView->RenderingContext;


    const BeginRenderPassInfo beginRenderPassInfo =
    {
        .RenderPass = RenderPasses.ForwardPass,
        .FrameBuffer = rContextView.ForwardFrameBuffer,
        .RenderOffSet = {0, 0},
        .Extent = {rContextView.RenderingContextSize.x, rContextView.RenderingContextSize.y},
        .ClearValueFlags = {},
        .ClearColor = nullptr,
        .ClearValueCount = 0,
        .ClearDepth = 1.f
    };

    auto forwardShader = ForwardShader.Lock();
    auto skyboxShader = SkyBoxShader.Lock();
    bool needForwardPass = forwardShader || skyboxShader;


    PrimaryCommandList->BeginDebugLabel("Forward Pass", FORWARD_DEBUG_COLOR);
    PrimaryCommandList->BeginRenderPass(beginRenderPassInfo);


    if (forwardShader && false)
    {
        PrimaryCommandList->BindProgram(forwardShader.get());
        PrimaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleList);

        PrimaryCommandList->SetViewPort(_viewportInfo);
        PrimaryCommandList->BindDescriptorSet(forwardShader.get(), rContextView.ForwardDesritptorSet,
                                              SCENE_DESCRIPTOR_SET, 1);

        // draw all static mesh
        DrawStaticMesh(MaterialType::Transparent, forwardShader);
    }

    if (skyboxShader && false)
    {
        PrimaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleList);

        PrimaryCommandList->BindProgram(skyboxShader.get());
        PrimaryCommandList->BindDescriptorSet(skyboxShader.get(), m_SkyboxCameraDescriptorSet,
                                              SCENE_DESCRIPTOR_SET, 1);
        PrimaryCommandList->BindDescriptorSet(skyboxShader.get(), m_SkyBoxCubeMapDescriptorSet,
                                              ENVIRONEMENT_DESCRIPTOR_SET, 1);
        PrimaryCommandList->BindVertexBuffer(*m_CubeVertexBuffer.GetRhiBuffer(), 0, 1);
        PrimaryCommandList->Draw(m_CubeVertexBuffer.GetVertexCount(), 1, 0, 0);
    }
#ifdef WITH_EDITOR
    m_DebugDrawContext->DrawDebugPrimitive(PrimaryCommandList.get(), rContextView);
    for (auto& it : UserCustomForwardPass)
        it(*this, PrimaryCommandList.get(), rContextView, &RenderWorldData);
#endif


    PrimaryCommandList->EndRenderPass();
    PrimaryCommandList->EndDebugLabel();
}

void Renderer::DefferdPass(const ViewportInfo& _viewportInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    const auto& rContextView = m_CurrentView->RenderingContext;

    auto clearValueFlags = static_cast<ClearValueFlags>(ClearValueColor |
        ClearValueDepth);

    // + 1 for depth 
    std::array<Tbx::Vector4f, static_cast<uint8_t>(GbufferType::Count) + 1> clearValues2 = {
        Tbx::Vector4f(0, 1, 0, 1.f),
    };

    const BeginRenderPassInfo beginRenderPassInfo =
    {
        .RenderPass = RenderPasses.DefferedPass,
        .FrameBuffer = rContextView.GbufferFrameBuffer,
        .RenderOffSet = {0, 0},
        .Extent = {rContextView.RenderingContextSize.x, rContextView.RenderingContextSize.y},
        .ClearValueFlags = clearValueFlags,
        .ClearColor = clearValues2.data(),
        .ClearValueCount = clearValues2.size(),
        .ClearDepth = 1.f
    };

    ObjectPtr sGeometry = GeometryBufferShader.lock();
    ObjectPtr sDeferred = DeferedShader.lock();


    PrimaryCommandList->BeginRenderPass(beginRenderPassInfo);
    PrimaryCommandList->BeginDebugLabel("Gbuffer Pass", GEOMETRY_PASS_COLOR);
    if (sGeometry)
    {
        PrimaryCommandList->BindProgram(sGeometry.get());
        PrimaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleList);
        PrimaryCommandList->BindDescriptorSet(sGeometry.get(), rContextView.GeometryDescritproSet, SCENE_DESCRIPTOR_SET,
                                              1);
        DrawStaticMesh(MaterialType::Opaque, sGeometry);
    }
    PrimaryCommandList->EndDebugLabel();
    PrimaryCommandList->NextSubPass();

    if (sDeferred)
    {
        PrimaryCommandList->BeginDebugLabel("DeferredPass", DEFERD_PASS_COLOR);
        PrimaryCommandList->BindProgram(sDeferred.get());
        PrimaryCommandList->BindDescriptorSet(sDeferred.get(), rContextView.DefferdLightingLightingCameraSet,
                                              SCENE_DESCRIPTOR_SET, 1);
        PrimaryCommandList->BindDescriptorSet(sDeferred.get(), rContextView.DefferdLightingGbufferSet, GBUFFER_SET, 1);

        PrimaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleStrip);
        PrimaryCommandList->Draw(4, 1, 0, 0);
        PrimaryCommandList->EndDebugLabel();
    }

    PrimaryCommandList->EndRenderPass();
}

PC_CORE_API void Renderer::PostProcess(const ViewportInfo& _viewportInfo)
{
    const auto& rContextView = m_CurrentView->RenderingContext;


    PrimaryCommandList->BeginDebugLabel("PostProcess Pass", POST_PROCESS);

    if (auto aces = AcesShader.lock().get())
    {
        PrimaryCommandList->BindProgram(aces);
        PrimaryCommandList->BindDescriptorSet(aces, rContextView.ToneMapDescritptorSet, 0, 1);

        const LocalSize& localSize = aces->GetLocalSize();

        uint32_t groupX = (static_cast<uint32_t>(_viewportInfo.Size.x) + localSize.x - 1) / localSize.x;
        uint32_t groupY = static_cast<uint32_t>(_viewportInfo.Size.y);


        const ImageMemoryBarrier gbufferImage =
        {
            .SrcAccessMask = GpuAccessFlag::ColorAttachmentWrite,
            .DstAccessMask = static_cast<GpuAccessFlag>(GpuAccessFlag::ShaderRead & GpuAccessFlag::ShaderWrite),

            .CurrentState = ImageState::RenderTargetOptimal,
            .NewState = ImageState::General,

            .Texture = rContextView.HdrImage->GetRhiTexture2D().get(),
        };

        PrimaryCommandList->Barrier(GpuPipelineStageFlagBits::ColorAttachmentOutput,
                                    GpuPipelineStageFlagBits::ComputeShader,
                                    nullptr, 0,
                                    nullptr, 0,
                                    &gbufferImage, 1);

        PrimaryCommandList->Dispatch(groupX, groupY, 1);

        assert(rContextView.HdrImage != nullptr);
    }
    PrimaryCommandList->EndDebugLabel();
}


void Renderer::FinalPass(const ViewportInfo& _viewportInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    std::array<Tbx::Vector4f, 2> clearValues2 = {
        Tbx::Vector4f(0, 0, 0, 0.f),
        Tbx::Vector4f(0, 0, 0, 0.f),
    };

    const auto& rContextView = m_CurrentView->RenderingContext;

    const BeginRenderPassInfo drawToViewport =
    {
        .RenderPass = RenderPasses.DrawToFinalViewPort,
        .FrameBuffer = rContextView.FinalImageFrameBuffer,
        .RenderOffSet = {0, 0},
        .Extent = {rContextView.RenderingContextSize.x, rContextView.RenderingContextSize.y},
        .ClearValueFlags = (ClearValueColor),
        .ClearColor = clearValues2.data(),
        .ClearValueCount = clearValues2.size(),
        .ClearDepth = 0.f,
        .ClearStencil = 0.f
    };
    PrimaryCommandList->BeginDebugLabel("Final Pass", FINAL_RENDER_PASS_DEBUG_COLOR);
    PrimaryCommandList->BeginRenderPass(drawToViewport);
    if (auto drawToViewPort = DrawTextureScreenQuadShader.Lock())
    {
        PrimaryCommandList->BindProgram(drawToViewPort.get());
        PrimaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleStrip);

        PrimaryCommandList->BindDescriptorSet(drawToViewPort.get(),
                                              rContextView.FinalImageDescritptorSet, 0, 1);
        PrimaryCommandList->Draw(4, 1, 0, 0);
    }
    PrimaryCommandList->EndRenderPass();
    PrimaryCommandList->EndDebugLabel();
}


#pragma region CreateRenderPasss


void Renderer::CreateRenderPasss()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    {
        PERF_REGION_SCOPED_NAMED("Create Defferd RenderPass");

        std::vector<RenderPassAttachementDescriptor> attachements;
        // + 1 lit image
        attachements.resize(
            static_cast<std::vector<RenderPassAttachementDescriptor>::size_type>(GbufferType::Count) + 1);

        attachements[static_cast<uint8_t>(GbufferType::Albedo)] =
        {
            .attachmentType = AttachmentType::Color,
            .format = RhiFormat::R16G16B16A16Sfloat,
            .sampleCount = 1,
            .load = LoadOperation::Clear,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
            .currentImageState = ImageState::Undefined,
            .finalImageState = ImageState::RenderTargetOptimal,
        };
        attachements[static_cast<uint8_t>(GbufferType::Normal)] =
        {
            .attachmentType = AttachmentType::Color,
            .format = RhiFormat::R16G16Snorm,
            .sampleCount = 1,
            .load = LoadOperation::Clear,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
            .currentImageState = ImageState::Undefined,
            .finalImageState = ImageState::RenderTargetOptimal,
        };
        attachements[static_cast<uint8_t>(GbufferType::RoughnessMetallicAo)] =
        {
            .attachmentType = AttachmentType::Color,
            .format = RhiFormat::R8G8B8A8Unorm,
            .sampleCount = 1,
            .load = LoadOperation::Clear,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
            .currentImageState = ImageState::Undefined,
            .finalImageState = ImageState::RenderTargetOptimal,
        };
        attachements[static_cast<uint8_t>(GbufferType::WorldPosition)] =
        {
            .attachmentType = AttachmentType::Color,
            .format = RhiFormat::R16G16B16A16Sfloat,
            .sampleCount = 1,
            .load = LoadOperation::Clear,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
            .currentImageState = ImageState::Undefined,
            .finalImageState = ImageState::RenderTargetOptimal,
        };
        RenderPassAttachementDescriptor depthAttachement =
        {
            .attachmentType = AttachmentType::Depth,
            .format = RhiFormat::D32Sfloat,
            .sampleCount = 1,
            .load = LoadOperation::Clear,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
            .currentImageState = ImageState::Undefined,
            .finalImageState = ImageState::DepthStencilOptimal,
        };

        // out image
        attachements[attachements.size() - 1] =
        {
            .attachmentType = AttachmentType::Color,
            .format = RhiFormat::R16G16B16A16Sfloat,
            .sampleCount = 1,
            .load = LoadOperation::Clear,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
            .currentImageState = ImageState::Undefined,
            .finalImageState = ImageState::RenderTargetOptimal,
        };

        // Geometry subpass and deffered lighting
        std::vector<SubPassDescription> subPassDescriptions;
        subPassDescriptions.resize(2);

        subPassDescriptions[0] =
        {
            .shaderProgramPipelineType = ShaderProgramPipelineType::Graphic,
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
            .shaderProgramPipelineType = ShaderProgramPipelineType::Graphic,
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
        RenderPassDescriptor renderPassDescriptor =
        {
            .attachement = attachements,
            .depthAttachment = &depthAttachement,
            .subPasses = subPassDescriptions
        };

        RenderPasses.DefferedPass = Rhi::CreateRenderPass(renderPassDescriptor);
    }

    // Forward
    {
        PERF_REGION_SCOPED_NAMED("Create Forward RenderPass");

        std::vector<RenderPassAttachementDescriptor> colorAttachement;
        colorAttachement.resize(1);

        colorAttachement[0] =
        {
            .attachmentType = AttachmentType::Color,
            .format = RhiFormat::R16G16B16A16Sfloat,
            .sampleCount = 1,
            .load = LoadOperation::Load,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
            .currentImageState = ImageState::RenderTargetOptimal,
            .finalImageState = ImageState::RenderTargetOptimal,
        };

        RenderPassAttachementDescriptor depthAttachement =
        {
            .attachmentType = AttachmentType::Depth,
            .format = RhiFormat::D32Sfloat,
            .sampleCount = 1,
            .load = LoadOperation::Load,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
            .currentImageState = ImageState::DepthStencilOptimal,
            .finalImageState = ImageState::DepthStencilOptimal,
        };

        std::vector<SubPassDescription> subPassDescriptions;
        subPassDescriptions.resize(1);

        subPassDescriptions[0] =
        {
            .shaderProgramPipelineType = ShaderProgramPipelineType::Graphic,
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

        RenderPassDescriptor renderPassDescriptor =
        {
            .attachement = colorAttachement,
            .depthAttachment = &depthAttachement,
            .subPasses = subPassDescriptions
        };

        RenderPasses.ForwardPass = Rhi::CreateRenderPass(renderPassDescriptor);
    }

    // Draw To Final Viewport
    {
        PERF_REGION_SCOPED_NAMED("Create Draw To Final Viewport");
        RenderPasses.DrawToFinalViewPort = Rhi::CreateRenderPass(RhiFormat::R8G8B8A8Unorm,
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
                .depthCompareOp = CompareOp::Less,
                .enableDepthTest = true
            },
            .vertexInputBindingDescritions =
            {
                StaticMeshVertex::GetVertexBindingDescription(0)
            },
            .vertexAttributeDescriptions = StaticMeshVertex::GetAttributeDescriptions(0),
        };

        const SourceList sources =
        {
            {
                ShaderStageType::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("Geometry.vs.hlsl.binary"),
            },
            {
                ShaderStageType::Pixel,
                ResourceManager::Get<ShaderSourceBinary>("Geometry.ps.hlsl.binary")
            }
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
            .shaderGraphicPointInfo = shaderGraphicPointInfo,
            .sourceList = sources,
            .renderPass = RenderPasses.DefferedPass.get(),
            // COUNT because we avoid depth but there is still final image 
            .colorAttachementCount = 4,
            .subPassIndex = 0,
        };

        GeometryBufferShader = ResourceManager::Create<GraphicShader>("Geometry", graphicShaderProgramCreateInfo);
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
                .depthCompareOp = CompareOp::Never,
                .enableDepthTest = true
            },
            .vertexInputBindingDescritions = {},
            .vertexAttributeDescriptions = {},
        };

        const SourceList sources =
        {
            {
                ShaderStageType::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("DrawQuad.vs.hlsl.binary"),
            },
            {
                ShaderStageType::Pixel,
                ResourceManager::Get<ShaderSourceBinary>("Deferred.ps.hlsl.binary")
            }
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
            .shaderGraphicPointInfo = shaderGraphicPointInfo,
            .sourceList = sources,
            .renderPass = RenderPasses.DefferedPass.get(),
            .colorAttachementCount = 1,
            .subPassIndex = 1,
        };

        DeferedShader = ResourceManager::Create<GraphicShader>("Deferred", graphicShaderProgramCreateInfo);
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
                .depthCompareOp = CompareOp::Less,
                .enableDepthTest = true
            },
            .vertexInputBindingDescritions = {StaticMeshVertex::GetVertexBindingDescription(0)},
            .vertexAttributeDescriptions = StaticMeshVertex::GetAttributeDescriptions(0),
        };

        const SourceList sources =
        {
            {
                ShaderStageType::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("Forward.vs.hlsl.binary"),
            },
            {
                ShaderStageType::Pixel,
                ResourceManager::Get<ShaderSourceBinary>("Forward.ps.hlsl.binary")
            }
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
            .shaderGraphicPointInfo = shaderGraphicPointInfo,
            .sourceList = sources,
            .renderPass = RenderPasses.ForwardPass.get(),
            .colorAttachementCount = 1,
            .subPassIndex = 0
        };

        ForwardShader = ResourceManager::Create<GraphicShader>("ForwardShader", graphicShaderProgramCreateInfo);
    }

    // SkyBox Shader
    {
        PERF_REGION_SCOPED_NAMED("SkyBox Shader");

        constexpr RasterizerInfo rasterizerInfo =
        {
            .polygonMode = PolygonMode::Fill,
            .cullModeFlag = CullModeFlagBit::None,
            .frontFace = FrontFace::CounterClockwise,
            .multiSampleRasterization = 1
        };

        constexpr VertexAttributeDescription vertexAttributeDescription =
        {
            .Binding = 0,
            .Location = 0,
            .Format = RhiFormat::R32G32B32Sfloat,
            .Offset = 0
        };

        constexpr VertexInputBindingDescrition vertexInputBindingDescrition =
        {
            .Binding = 0,
            .Stride = sizeof(Tbx::Vector3f),
            .VertexInputRate = VertexInputRate::Vertex,
        };


        const ShaderGraphicPointInfo shaderGraphicPointInfo =
        {
            .rasterizerInfo = rasterizerInfo,
            .dephInfo =
            {
                .depthCompareOp = CompareOp::LessOrEqual,
                .enableDepthTest = true
            },
            .vertexInputBindingDescritions = {vertexInputBindingDescrition},
            .vertexAttributeDescriptions = {vertexAttributeDescription},
        };

        const SourceList source =
        {
            {
                ShaderStageType::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("Skybox.vs.hlsl.binary")
            },
            {
                ShaderStageType::Pixel,
                ResourceManager::Get<ShaderSourceBinary>("Skybox.ps.hlsl.binary")
            }
        };


        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
            .shaderGraphicPointInfo = shaderGraphicPointInfo,
            .sourceList = source,
            .renderPass = RenderPasses.ForwardPass.get(),
            .colorAttachementCount = 1,
            .subPassIndex = 0

        };


        SkyBoxShader = ResourceManager::Create<GraphicShader>(
            "SkyboxShader", graphicShaderProgramCreateInfo);
    }

    {
        PERF_REGION_SCOPED_NAMED("ToneMap Shader");

        const ComputeShaderProgramCreateInfo computeShaderProgramCreateInfo =
        {
            .shaderComputeInfo = {},
            .source = ResourceManager::Get<ShaderSourceBinary>("Aces.cs.hlsl.binary")
        };

        AcesShader = ResourceManager::Create<ComputeShader>("Aces", computeShaderProgramCreateInfo);
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
                .depthCompareOp = CompareOp::Less,
                .enableDepthTest = true
            },
            .vertexInputBindingDescritions = {},
            .vertexAttributeDescriptions = {},
        };

        const SourceList sources =
        {
            {
                ShaderStageType::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("DrawQuad.vs.hlsl.binary"),
            },
            {
                ShaderStageType::Pixel,
                ResourceManager::Get<ShaderSourceBinary>("SampleSingleTexture.ps.hlsl.binary")
            }
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
            .shaderGraphicPointInfo = shaderGraphicPointInfo,
            .sourceList = sources,
            .renderPass = RenderPasses.DrawToFinalViewPort.get(),
            .colorAttachementCount = 1,

        };


        DrawTextureScreenQuadShader = ResourceManager::Create<GraphicShader>(
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
        /*
        PERF_REGION_SCOPED_NAMED("Create Cube Map");
        std::array<std::string, 6> maps
        {
            "Assets/Textures/Skybox/Right.jpg",
            "Assets/Textures/Skybox/Left.jpg",
            "Assets/Textures/Skybox/Top.jpg",
            "Assets/Textures/Skybox/Bottom.jpg",
            "Assets/Textures/Skybox/Front.jpg",
            "Assets/Textures/Skybox/Back.jpg",
        };
        m_Cubemap = ResourceManager::Create<Texture3D>("BasicCubemap", maps);*/
    }
}

void Renderer::CreateBuffers()
{
    UniformBuffers.CameraUniformBuffer = UniformBuffer(sizeof(CameraGpu),
                                                       MemoryLocalisation::CpuToGpu, MemoryUsage::Dynamic);
    UniformBuffers.PostProcessUniformBuffer = UniformBuffer(&UniformBuffers.PostProcessUniformBuffer,
                                                            sizeof(PostProcessGpu),
                                                            MemoryLocalisation::CpuToGpu, MemoryUsage::Dynamic);

    m_GpuLightUniformBufferStaging = UniformBuffer(&UniformBuffers.PostProcessUniformBuffer,
                                                 sizeof(m_GpuDynamicLightData),
                                                 MemoryLocalisation::CpuToGpu, MemoryUsage::Dynamic);

    UniformBuffers.DynamicGpuLightUniformBuffer = UniformBuffer(&m_GpuDynamicLightData, sizeof(m_GpuDynamicLightData),
                                                                MemoryLocalisation::GpuOnly, MemoryUsage::Dynamic);
}
#pragma endregion CreateThirdPartyResources

#pragma region CreateDescriptorSets

void Renderer::CreateDescriptorSets()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    const UniformBufferDescriptor lightData
    {
        .buffer = &UniformBuffers.CameraUniformBuffer,
    };
    /*
    ImageSamplerDescriptor skyboxCubeMapDescritptor
    {
        .sampler = &m_SkyBoxSampler,
        .texture = m_Cubemap.lock().get(),
        .imageState = PC_CORE::ImageState::ShaderReadOptimal
    };*/

    std::vector<ShaderProgramDescriptorWrite> descriptorSets;

    {
        PERF_REGION_SCOPED_NAMED("Skybox Shader DescriptorSets");

        descriptorSets =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                lightData,
            },
        };
        SkyBoxShader.lock()->AllocDescriptorSet(&m_SkyboxCameraDescriptorSet, SCENE_DESCRIPTOR_SET);
        m_SkyboxCameraDescriptorSet->WriteDescriptorSets(descriptorSets);
        /*
        descriptorSets =
        {
            {
                ShaderProgramDescriptorType::CombinedImageSampler,
                SKYBOX_BINDING,
                skyboxCubeMapDescritptor,
            }
        };
        m_SkyBoxShader.lock()->AllocDescriptorSet(&skyBoxCubeMapDescriptorSet,
                                                   ENVIRONEMENT_DESCRIPTOR_SET);
        skyBoxCubeMapDescriptorSet->WriteDescriptorSets(descriptorSets);*/
    }
}
#pragma endregion CreateDescriptorSets

void Renderer::InitCubeBuffers()
{
    constexpr std::array<Tbx::Vector3f, 36> vertices =
    {
        // Bottom face 
        Tbx::Vector3f{-0.5f, -0.5f, -0.5f},
        Tbx::Vector3f{0.5f, -0.5f, -0.5f},
        Tbx::Vector3f{0.5f, -0.5f, 0.5f},
        Tbx::Vector3f{-0.5f, -0.5f, -0.5f},
        Tbx::Vector3f{0.5f, -0.5f, 0.5f},
        Tbx::Vector3f{-0.5f, -0.5f, 0.5f},

        // Top face 
        Tbx::Vector3f{-0.5f, 0.5f, -0.5f},
        Tbx::Vector3f{0.5f, 0.5f, 0.5f},
        Tbx::Vector3f{0.5f, 0.5f, -0.5f},
        Tbx::Vector3f{-0.5f, 0.5f, -0.5f},
        Tbx::Vector3f{-0.5f, 0.5f, 0.5f},
        Tbx::Vector3f{0.5f, 0.5f, 0.5f},

        // Front face 
        Tbx::Vector3f{-0.5f, -0.5f, 0.5f},
        Tbx::Vector3f{0.5f, -0.5f, 0.5f},
        Tbx::Vector3f{0.5f, 0.5f, 0.5f},
        Tbx::Vector3f{-0.5f, -0.5f, 0.5f},
        Tbx::Vector3f{0.5f, 0.5f, 0.5f},
        Tbx::Vector3f{-0.5f, 0.5f, 0.5f},

        // Back face 
        Tbx::Vector3f{-0.5f, -0.5f, -0.5f},
        Tbx::Vector3f{0.5f, 0.5f, -0.5f},
        Tbx::Vector3f{0.5f, -0.5f, -0.5f},
        Tbx::Vector3f{-0.5f, -0.5f, -0.5f},
        Tbx::Vector3f{-0.5f, 0.5f, -0.5f},
        Tbx::Vector3f{0.5f, 0.5f, -0.5f},

        // Left face 
        Tbx::Vector3f{-0.5f, -0.5f, -0.5f},
        Tbx::Vector3f{-0.5f, 0.5f, 0.5f},
        Tbx::Vector3f{-0.5f, 0.5f, -0.5f},
        Tbx::Vector3f{-0.5f, -0.5f, -0.5f},
        Tbx::Vector3f{-0.5f, -0.5f, 0.5f},
        Tbx::Vector3f{-0.5f, 0.5f, 0.5f},

        // Right face 
        Tbx::Vector3f{0.5f, -0.5f, -0.5f},
        Tbx::Vector3f{0.5f, 0.5f, -0.5f},
        Tbx::Vector3f{0.5f, 0.5f, 0.5f},
        Tbx::Vector3f{0.5f, -0.5f, -0.5f},
        Tbx::Vector3f{0.5f, 0.5f, 0.5f},
        Tbx::Vector3f{0.5f, -0.5f, 0.5f},
    };

    m_CubeVertexBuffer = VertexBuffer(vertices.data(), vertices.size(), sizeof(Tbx::Vector3f),
                                      MemoryLocalisation::GpuOnly, MemoryUsage::Static);
}
