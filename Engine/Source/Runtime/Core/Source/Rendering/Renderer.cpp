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
#include "Rendering/Sampler.hpp"

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

void Renderer::Init(Rhi& _Rhi)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    m_Rhi = &_Rhi;

    PrimaryCommandList.reset(m_Rhi->CreateCommandList());
    PrimaryCommandList
        ->SetBufferType(CommandList::BufferType::Primary)
        .SetPoolFamilly(CommandList::PoolFamily::Graphics)
        .SetName("PrimaryCommandList")
        .Build();
    /*
    SwapChainPassCommandList.reset(m_Rhi->CreateCommandList());
    SwapChainPassCommandList
        ->SetBufferType(CommandList::BufferType::Primary)
        .SetPoolFamilly(CommandList::PoolFamily::Graphics)
        .SetName("SwapChainPassCommandList")
        .Build();
        */
    LinearReapeat = Sampler(*m_Rhi, "LinearReapeat");
    LinearReapeat
        ->SetMagFilter(Filter::Linear)
        .SetMinFilter(Filter::Linear)
        .SetU(SamplerAddressMode::Repeat)
        .SetV(SamplerAddressMode::Repeat)
        .SetW(SamplerAddressMode::Repeat)
        .Build();
        
    /*
    m_SkyBoxSampler = Sampler(*m_Rhi, "SkyBoxSampler");
    m_SkyBoxSampler
        ->SetMagFilter(Filter::Linear)
        .SetMinFilter(Filter::Linear)
        .SetU(SamplerAddressMode::ClampToEdge)
        .SetV(SamplerAddressMode::ClampToEdge)
        .SetW(SamplerAddressMode::ClampToEdge)
        .Build();
    m_SkyBoxSampler->Build();*/


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

    m_Rhi->GetRhiContext().rhiSwapChain->GetSwapChainImageIndex(_window);

    PrimaryCommandList->Reset();
    PrimaryCommandList->BeginRecordCommands();
}

void Renderer::UpdateGpuCameraData()
{
    const auto& rContextView = m_CurrentView->RenderingContext;
    const auto& gpuCamera = m_CurrentView->CameraGpu;

    if (char* ptr = UniformBuffers.Camera->BeginFullDynamicBufferUpdateForCurrentFrame())
    {
        std::memcpy(ptr, &gpuCamera, sizeof(CameraGpu));

        UniformBuffers.Camera->EndFullDynamicBufferUpdateForCurrentFrame();
    }

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
    if (char* ptr = UniformBuffers.LightBuffer->BeginFullDynamicBufferUpdateForCurrentFrame())
    {
        std::memcpy(ptr, &m_GpuDynamicLightData, sizeof(m_GpuDynamicLightData));
        UniformBuffers.LightBuffer->EndFullDynamicBufferUpdateForCurrentFrame();
    }
}


void Renderer::Draw(const View& _view)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    m_CurrentView = &_view;


    UpdateGpuCameraData();
    
 
#ifdef WITH_EDITOR
    m_DebugDrawContext->Prepare();
#endif
    UpdateLightGpuData(PrimaryCommandList.get());

    const auto& rContextView = m_CurrentView->RenderingContext;
    const ViewportInfo viewportInfo(rContextView.RenderingContextSize);
    PrimaryCommandList->SetViewPort(viewportInfo);
    //DefferdPass(viewportInfo);
    ForwardPass(viewportInfo);
    //PostProcess(viewportInfo);
    FinalPass(viewportInfo);
}


void Renderer::SwapBuffers(Window* _window)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    std::shared_ptr<RhiSwapChain> rhiSwapChain = m_Rhi->GetRhiContext().rhiSwapChain;

    // THIS FUNCTION SHOULDE BE IN RHI
    rhiSwapChain->BeginSwapChainRenderPass(PrimaryCommandList.get());
    PrimaryCommandList->ExecuteExternalCommand();
    rhiSwapChain->EndSwapChainRenderPass(PrimaryCommandList.get());

    PrimaryCommandList->EndRecordCommands();
    PrimaryCommandList->Flush(FlushCommandMethod::Sync, GpuPipelineStage::ColorAttachmentOutput);

    ClearRenderData();
    rhiSwapChain->Present(_window);
    m_Rhi->Rhi::NextFrame();
}

std::shared_ptr<View> Renderer::CreateView(Tbx::Vector2i _defaultSize)
{
    auto view = std::make_shared<View>(this, _defaultSize);

    m_Views.push_back(view);

    return view;
}

Rhi& Renderer::GetRhi()
{
    return *m_Rhi;
}

void Renderer::DrawStaticMesh(MaterialType _type, RhiShaderProgram& _shader)
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

        PrimaryCommandList->BindDescriptorSet(_shader, materialDescriptor, MATERIAL_DESCRIPTOR_SET, 1);

        PrimaryCommandList->PushConstant(_shader, "pushConstant", &modelMatrixf,
                                         sizeof(Tbx::Matrix4x4f) * 2);
        PrimaryCommandList->BindVertexBuffer(*mesh->VBuffer.Get(), 0, 1);
        PrimaryCommandList->BindIndexBuffer(*mesh->IBuffer.Get(), mesh->IBuffer.GetIndexFormat(), 0);
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
    
   
    
    std::array<Tbx::Vector4f, 1> clearValues = {
      Tbx::Vector4f(0.1, 0.1, 0.1, 1.f),
    };
    const BeginRenderPassInfo beginRenderPassInfo =
    {
        .RenderPass = RenderPasses.ForwardPass,
        .FrameBuffer = m_CurrentView->FrameBuffers.ForwardFrameBuffer,
        .RenderOffSet = {0, 0},
        .Extent = {rContextView.RenderingContextSize.x, rContextView.RenderingContextSize.y},
        .ClearValueFlag = ClearValueFlagBits::ClearValueColor | ClearValueFlagBits::ClearValueDepth,
        .ClearColor = clearValues.data(),
        .ClearValueCount = clearValues.size(),
        .ClearDepth = 1.f
    };


    PrimaryCommandList->BeginDebugLabel("Forward Pass", FORWARD_DEBUG_COLOR);
    PrimaryCommandList->BeginRenderPass(beginRenderPassInfo);


    if (ForwardShader && false)
    {
        PrimaryCommandList->BindProgram(*ForwardShader.get());
        PrimaryCommandList->SetPrimitiveTopology(RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyTriangleList);

        PrimaryCommandList->SetViewPort(_viewportInfo);
        PrimaryCommandList->BindDescriptorSet(*ForwardShader.get(), 
                                              m_CurrentView->DescriptorSets.ForwardDescriptor.get(),
                                              SCENE_DESCRIPTOR_SET, 1);

        // draw all static mesh
        DrawStaticMesh(MaterialType::Transparent, *ForwardShader.get());
    }

    if (SkyBoxShader && false)
    {
        RhiShaderProgram& skyBoxShader = *SkyBoxShader;
        PrimaryCommandList->SetPrimitiveTopology(RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyTriangleList);

        PrimaryCommandList->BindProgram(skyBoxShader);
        PrimaryCommandList->BindDescriptorSet(skyBoxShader, m_SkyboxCameraDescriptorSet,
            SCENE_DESCRIPTOR_SET, 1);
        PrimaryCommandList->BindDescriptorSet(skyBoxShader, m_SkyBoxCubeMapDescriptorSet,
            ENVIRONEMENT_DESCRIPTOR_SET, 1);
        PrimaryCommandList->BindVertexBuffer(*m_CubeVertexBuffer, 0, 1);
        PrimaryCommandList->Draw(CubeVerticiesCount, 1, 0, 0);
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

    return; // TO DO HANDLE INPUT ATTACHEMNT
    /*
    const auto& rContextView = m_CurrentView->RenderingContext;

    auto clearValueFlags = static_cast<ClearValueFlag>(ClearValueColor |
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
        .ClearValueFlag = clearValueFlags,
        .ClearColor = clearValues2.data(),
        .ClearValueCount = clearValues2.size(),
        .ClearDepth = 1.f
    };



    RhiShaderProgram& sGeometry = *GeometryBufferShader;
    PrimaryCommandList->BeginRenderPass(beginRenderPassInfo);
    {
        PrimaryCommandList->BeginDebugLabel("Gbuffer Pass", GEOMETRY_PASS_COLOR);

        PrimaryCommandList->BindProgram(sGeometry);
        PrimaryCommandList->SetPrimitiveTopology(RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyTriangleList);
        PrimaryCommandList->BindDescriptorSet(sGeometry, rContextView.GeometryDescritproSet, SCENE_DESCRIPTOR_SET, 1);
        DrawStaticMesh(MaterialType::Opaque, sGeometry);
        PrimaryCommandList->EndDebugLabel();
    }
        
    RhiShaderProgram& sDeferred = *DeferedShader;
    PrimaryCommandList->NextSubPass();
    {
        PrimaryCommandList->BeginDebugLabel("DeferredPass", DEFERD_PASS_COLOR);
        PrimaryCommandList->BindProgram(sDeferred);
        PrimaryCommandList->BindDescriptorSet(sDeferred, rContextView.DefferdLightingLightingCameraSet,
                                              SCENE_DESCRIPTOR_SET, 1);
        PrimaryCommandList->BindDescriptorSet(sDeferred, rContextView.DefferdLightingGbufferSet, GBUFFER_SET, 1);

        PrimaryCommandList->SetPrimitiveTopology(RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyTriangleStrip);
        PrimaryCommandList->Draw(4, 1, 0, 0);
        PrimaryCommandList->EndDebugLabel();
    }

    PrimaryCommandList->EndRenderPass();*/
}

PC_CORE_API void Renderer::PostProcess(const ViewportInfo& _viewportInfo)
{
    /*
    const auto& rContextView = m_CurrentView->RenderingContext;


    PrimaryCommandList->BeginDebugLabel("PostProcess Pass", POST_PROCESS);

    if (auto aces = AcesShader.lock().get())
    {
        PrimaryCommandList->BindProgram(aces);
        PrimaryCommandList->BindDescriptorSet(aces, rContextView.ToneMapDescritptorSet, 0, 1);

        const LocalSize& localSize = aces->GetLocalSize();

        uint32_t groupX = (static_cast<uint32_t>(_viewportInfo.Size.x) + localSize.x - 1) / localSize.x;
        uint32_t groupY = static_cast<uint32_t>(_viewportInfo.Size.y);


        const ImageBarrier gbufferImage =
        {
            .CurrentState = RhiResourceState::RenderTarget,
            .NewState = RhiResourceState::ComputeWrite,

            .Texture = rContextView.HdrImage->Get(),
        };

        PrimaryCommandList->Barrier(GpuPipelineStageFlagBits::ColorAttachmentOutput,
                                    GpuPipelineStageFlagBits::ComputeShader,
                                    nullptr, 0,
                                    nullptr, 0,
                                    &gbufferImage, 1);

        PrimaryCommandList->Dispatch(groupX, groupY, 1);

        const ImageBarrier gbufferImage2 =
        {
            .CurrentState = RhiResourceState::ComputeWrite,
            .NewState = RhiResourceState::FragmentShaderResource,

            .Texture = rContextView.HdrImage->Get(),
        };

        PrimaryCommandList->Barrier(
            GpuPipelineStageFlagBits::ComputeShader,
            GpuPipelineStageFlagBits::ColorAttachmentOutput,
            nullptr, 0,
            nullptr, 0,
            &gbufferImage2, 1);

        assert(rContextView.HdrImage != nullptr);
    }
    PrimaryCommandList->EndDebugLabel();*/
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
        .FrameBuffer = m_CurrentView->FrameBuffers.FinalImageFrameBuffer,
        .RenderOffSet = {0, 0},
        .Extent = {rContextView.RenderingContextSize.x, rContextView.RenderingContextSize.y},
        .ClearValueFlag = (ClearValueColor),
        .ClearColor = clearValues2.data(),
        .ClearValueCount = clearValues2.size(),
        .ClearDepth = 0.f,
        .ClearStencil = 0.f
    };
    PrimaryCommandList->BeginDebugLabel("Final Pass", FINAL_RENDER_PASS_DEBUG_COLOR);
    PrimaryCommandList->BeginRenderPass(drawToViewport);
    if (DrawTextureScreenQuadShader)
    {
        PrimaryCommandList->BindProgram(*DrawTextureScreenQuadShader);
        PrimaryCommandList->SetPrimitiveTopology(RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyTriangleStrip);

        PrimaryCommandList->BindDescriptorSet(
            *DrawTextureScreenQuadShader,
            m_CurrentView->DescriptorSets.FinalViewPort.get(),
            0, 
            1);
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
        /*
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
            .currentImageState = RhiResourceState::Undefined,
            .finalImageState = RhiResourceState::RenderTarget,
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
            .currentImageState = RhiResourceState::Undefined,
            .finalImageState = RhiResourceState::RenderTarget,
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
            .currentImageState = RhiResourceState::Undefined,
            .finalImageState = RhiResourceState::RenderTarget,
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
            .currentImageState = RhiResourceState::Undefined,
            .finalImageState = RhiResourceState::RenderTarget,
        };
        RenderPassAttachementDescriptor depthAttachement =
        {
            .attachmentType = AttachmentType::Depth,
            .format = RhiFormat::D24UnormS8Uint,
            .sampleCount = 1,
            .load = LoadOperation::Clear,
            .store = StoreOperation::Store,
            .stencilLoad = LoadOperation::DontCare,
            .stencilStore = StoreOperation::DontCare,
            .currentImageState = RhiResourceState::Undefined,
            .finalImageState = RhiResourceState::DepthStencilWrite,
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
            .currentImageState = RhiResourceState::Undefined,
            .finalImageState = RhiResourceState::RenderTarget,
        };

        // Geometry subpass and deffered lighting
        std::vector<SubPass> subPassDescriptions;
        subPassDescriptions.resize(2);

        subPassDescriptions[0] =
        {
            .type = PipelineType::Graphic,
            .ColorAttachements = {
                static_cast<size_t>(GbufferType::Albedo),
                static_cast<size_t>(GbufferType::Normal),
                static_cast<size_t>(GbufferType::RoughnessMetallicAo),
                static_cast<size_t>(GbufferType::WorldPosition)
            },
            .InputAttachements = {},
            .subPassAccess =
            {
                .SrcStageFlag = GpuPipelineStage::ColorAttachmentOutput | GpuPipelineStage::EarlyFragmentTests,
                .DstStageFlag = GpuPipelineStage::FragmentShader,
                .ImageStateTransition =
                    {
                        .OldState = RhiResourceState::RenderTarget,
                        .NewState = RhiResourceState::FragmentShaderResource
                    }
            },
            .useDepth = true,
        };
        subPassDescriptions[1] =
        {
            .type = PipelineType::Graphic,
            .ColorAttachements =
            {
                attachements.size() - 1
            },
            .InputAttachements = {
                static_cast<size_t>(GbufferType::Albedo),
                static_cast<size_t>(GbufferType::Normal),
                static_cast<size_t>(GbufferType::RoughnessMetallicAo),
                static_cast<size_t>(GbufferType::WorldPosition)
            },
            .subPassAccess =
            {
                .SrcStageFlag = GpuPipelineStage::ColorAttachmentOutput | GpuPipelineStage::EarlyFragmentTests,
                .DstStageFlag = GpuPipelineStage::FragmentShader,
                .ImageStateTransition =
                    {
                        .OldState = RhiResourceState::RenderTarget,
                        .NewState = RhiResourceState::FragmentShaderResource
                    }
            },
            .useDepth = false,
        };
        RenderPassDescriptor renderPassDescriptor =
        {
            .attachement = attachements,
            .depthAttachment = &depthAttachement,
            .subPasses = subPassDescriptions
        };

        RenderPasses.DefferedPass.reset(Rhi::CreateRenderPass("DefferedRenderPass", renderPassDescriptor));*/
    }

    // Forward
    {
        PERF_REGION_SCOPED_NAMED("Create Forward RenderPass");

        RenderPasses.ForwardPass.reset(m_Rhi->CreateRenderPass());
        RhiRenderPass& forwardPass = *RenderPasses.ForwardPass;

        // Color
        const RenderPassAttachementDescriptor& Albedo = forwardPass
            .CreateAttachment()
            .SetAttachementSlot(AttachementSlot::S00)
            .SetRhiFormat(RhiFormat::R16G16B16A16Sfloat)
            .SetSampleCount(1)
            .SetLoadOp(LoadOperation::Clear)
            .SetStoreOp(StoreOperation::Store)
            .SetInitialImageState(RhiResourceState::Undefined)
            .SetFinalImageState(RhiResourceState::FragmentShaderResource);

        // Set Depth
        const RenderPassAttachementDescriptor& DepthBuffer = forwardPass
            .CreateAttachment()
            .SetAttachementSlot(AttachementSlot::S01)
            .SetRhiFormat(RhiFormat::D24UnormS8Uint)
            .SetSampleCount(1)
            .SetLoadOp(LoadOperation::Clear)
            .SetStoreOp(StoreOperation::Store)
            .SetInitialImageState(RhiResourceState::DepthStencilWrite)
            .SetFinalImageState(RhiResourceState::DepthStencilWrite);

        // SubPass 0
        forwardPass
            .CreateSubPass()
            .SetType(RhiShaderProgram::PipelineType::Graphic)
            .SetAttachementRef(AttachementRef(Albedo, RhiResourceState::RenderTarget))
            .SetDepthAttachementRef(AttachementRef(DepthBuffer, RhiResourceState::DepthStencilWrite))
            .SetSrcStageFlag({})
            .SetDstStageFlag({});

        forwardPass
            .SetName("ForwardPass")
            .Build();

    }

    // Draw To Final Viewport
    {
        PERF_REGION_SCOPED_NAMED("Create Draw To Final Viewport");
        RenderPasses.DrawToFinalViewPort.reset(m_Rhi->CreateRenderPass(RhiFormat::R8G8B8A8Unorm,
                                                                 m_Rhi->GetRhiContext().rhiPhysicalDevices->
                                                                 GetPhysicalDevice().GetMaxUsableSampleCount()));
        RenderPasses.DrawToFinalViewPort
            ->SetName("DrawToFinalViewPort");
            //.Build(); // DO NOT CALL IT

    }
}


#pragma endregion CreateRenderPasss

#pragma region CreateShaders
void Renderer::CreateShaders()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);


    {
        /*
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
        GeometryBufferShader.Lock()->Get()->Build();
        */
    }

    {
        /*
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
        DeferedShader.Lock()->Get()->Build();
        */
    }

    {
        PERF_REGION_SCOPED_NAMED("Forward Shader");
        
        const std::vector<RhiShaderProgram::ShaderModule> moldules =
        {
            {
                RhiShaderProgram::ShaderStageType::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("Forward.vs.hlsl.binary")->GetCode(),
            },
            {
                RhiShaderProgram::ShaderStageType::Pixel,
                ResourceManager::Get<ShaderSourceBinary>("Forward.ps.hlsl.binary")->GetCode()
            }
        };

        ForwardShader.reset(m_Rhi->CreateRhiShaderProgram());
        ForwardShader->SetShaderModules(moldules)
            .SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
            .SetDepthTest(true)
            .SetDepthWrite(true)
            .SetCullMode(RhiShaderProgram::CullBack)
            .SetVertexInputBindingDescritions({ StaticMeshVertex::GetVertexBindingDescription(0)})
            .SetVertexAttributeDescriptions(StaticMeshVertex::GetAttributeDescriptions(0))
            .SetRenderPass(*RenderPasses.ForwardPass)
            .SetAttachementCount(1) // shoulde be in renderpass
            .SetSubPassIndex(0)
            .SetName("ForwardShader")
            .Build();
        
    }

    // SkyBox Shader
    {
        PERF_REGION_SCOPED_NAMED("SkyBox Shader");
        /*
        constexpr RhiShaderProgram::RasterizerInfo rasterizerInfo =
        {
            .polygonMode = RhiShaderProgram::PolygonMode::Fill,
            .cullModeFlag = RhiShaderProgram::CullModeFlagBit::None,
            .frontFace = RhiShaderProgram::FrontFace::CounterClockwise,
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


        const RhiShaderProgram::ShaderGraphicPointInfo shaderGraphicPointInfo =
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
                RhiShaderProgram::ShaderStageType::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("Skybox.vs.hlsl.binary")
            },
            {
                RhiShaderProgram::ShaderStageType::Pixel,
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
        
        SkyBoxShader.Lock()->Get()->Build();*/
    }

    {
        PERF_REGION_SCOPED_NAMED("ToneMap Shader");
        
        const std::vector<RhiShaderProgram::ShaderModule> modules =
        {
            {
                RhiShaderProgram::ShaderStageType::Compute,
                ResourceManager::Get<ShaderSourceBinary>("Aces.cs.hlsl.binary")->GetCode()
            }
        };

        AcesShader.reset(m_Rhi->CreateRhiShaderProgram());
        AcesShader
            ->SetPipelineType(RhiShaderProgram::PipelineType::Compute)
            .SetShaderModules(modules)
            .SetName("ToneMap Shader")
            .Build();
    }

    // Draw to final viewport
    {
        PERF_REGION_SCOPED_NAMED("CreateDrawToFinalViewport Programm");

        const std::vector<RhiShaderProgram::ShaderModule> modules =
        {
            {
                RhiShaderProgram::ShaderStageType::Vertex,
                ResourceManager::Get<ShaderSourceBinary>("DrawQuad.vs.hlsl.binary")->GetCode(),
            },
            {
                RhiShaderProgram::ShaderStageType::Pixel,
                ResourceManager::Get<ShaderSourceBinary>("SampleSingleTexture.ps.hlsl.binary")->GetCode()
            }
        };

        DrawTextureScreenQuadShader.reset(m_Rhi->CreateRhiShaderProgram());
        DrawTextureScreenQuadShader
            ->SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
            .SetSamples(m_Rhi->GetRhiContext().rhiPhysicalDevices->GetPhysicalDevice()
            .GetMaxUsableSampleCount())
            .SetAttachementCount(1)
            .SetSubPassIndex(0)
            .SetRenderPass(*RenderPasses.DrawToFinalViewPort)
            .SetShaderModules(modules)
            .SetName("DrawQuadShader")
            .Build();
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
            "C:/Data/ParaConquerGame/Assets/Textures/Skybox/Right.jpg",
            "C:/Data/ParaConquerGame/Assets/Textures/Skybox/Left.jpg",
            "C:/Data/ParaConquerGame/Assets/Textures/Skybox/Top.jpg",
            "C:/Data/ParaConquerGame/Assets/Textures/Skybox/Bottom.jpg",
            "C:/Data/ParaConquerGame/Assets/Textures/Skybox/Front.jpg",
            "C:/Data/ParaConquerGame/Assets/Textures/Skybox/Back.jpg",
        };
        Cubemap = ResourceManager::Create<Texture3D>(*m_Rhi, "BasicCubemap", maps);
    }
}

void Renderer::CreateBuffers()
{
    UniformBuffers.Camera.reset(m_Rhi->CreateBuffer());
    UniformBuffers.Camera
        ->SetMemoryUsage(RhiMemoryUsage::Dynamic)
        .SetSize(sizeof(CameraGpu))
        .SetUsage(RhiBuffer::Uniform)
        .SetName("CameraUniformBuffer")
        .Build();

    UniformBuffers.PostProcess.reset(m_Rhi->CreateBuffer());
    UniformBuffers.PostProcess
        ->SetMemoryUsage(RhiMemoryUsage::Dynamic)
        .SetSize(sizeof(PostProcessGpu))
        .SetUsage(RhiBuffer::Uniform)
        .SetName("PostProcessUniformBuffer")
        .Build();

    UniformBuffers.LightBuffer.reset(m_Rhi->CreateBuffer());
    UniformBuffers.LightBuffer
        ->SetMemoryUsage(RhiMemoryUsage::Dynamic)
        .SetSize(sizeof(m_GpuDynamicLightData))
        .SetUsage(RhiBuffer::Uniform)
        .SetName("DynamicGpuLightUniformBuffer")
        .Build();
}
#pragma endregion CreateThirdPartyResources

#pragma region CreateDescriptorSets

void Renderer::CreateDescriptorSets()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    const BufferDescriptor lightData
    {
        .buffer = UniformBuffers.Camera.get(),
    };
    /*
    ImageSamplerDescriptor skyboxCubeMapDescritptor
    {
        .sampler = &m_SkyBoxSampler,
        .texture = m_Cubemap.lock().get(),
        .resourceState = PC_CORE::ImageState::ShaderReadOptimal
    };*/

    std::vector<DescriptorWrite> DescriptorSets;

    {
        PERF_REGION_SCOPED_NAMED("Skybox Shader DescriptorSets");
        /*
        DescriptorSets =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                lightData,
            },
        };
        SkyBoxShader.lock()->CreateDescriptorBinding(&m_SkyboxCameraDescriptorSet, SCENE_DESCRIPTOR_SET);
        m_SkyboxCameraDescriptorSet->SetBindings(DescriptorSets);
     
        DescriptorSets =
        {
            {
                ShaderProgramDescriptorType::CombinedImageSampler,
                SKYBOX_BINDING,
                skyboxCubeMapDescritptor,
            }
        };
        m_SkyBoxShader.lock()->CreateDescriptorBinding(&skyBoxCubeMapDescriptorSet,
                                                   ENVIRONEMENT_DESCRIPTOR_SET);
        skyBoxCubeMapDescriptorSet->SetBindings(DescriptorSets);*/
    }
}
#pragma endregion CreateDescriptorSets

void Renderer::InitCubeBuffers()
{
    constexpr std::array<Tbx::Vector3f, CubeVerticiesCount> vertices =
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

    m_CubeVertexBuffer.reset(m_Rhi->CreateBuffer());
    m_CubeVertexBuffer
        ->SetMemoryUsage(RhiMemoryUsage::Static)
        .SetSize(sizeof(sizeof(Tbx::Vector3f)) * vertices.size())
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Vertex | RhiBuffer::BufferUsageFlagBits::TransferDst)
        .Build();
    m_CubeVertexBuffer->UploadData(nullptr, vertices.data(), vertices.size());
    /*
    m_Rhi->PushResourceUpdate([&](CommandList* list)
    {
            m_CubeVertexBuffer->UploadData(list, vertices.data(), vertices.size());
    }) ;*/
}
