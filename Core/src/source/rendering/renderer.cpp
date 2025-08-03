#include "rendering/renderer.hpp"

#include <thread>
#include <perf_region.hpp>

#include "low_renderer/rhi.hpp"
#include "low_renderer/vertex.hpp"
#include "resources/resource_manager.hpp"
#include "time/core_time.hpp"
#include "math/matrix_transformation.hpp"
#include "world/static_mesh.hpp"
#include "world/transform.hpp"
#include "rendering/render_passes/render_pass.hpp"


#include "math/toolbox_typedef.hpp"
#include "rendering/render_system.hpp"
#include "resources/shader_source_binary.hpp"

#include "vulkan_command_list.hpp"
#include "resources/compute_shader.hpp"

using namespace PC_CORE;


void Renderer::GetRenderingData(const RenderingWorldData& _newRenderingData)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);


    renderWorldData.Clear();
    renderWorldData = _newRenderingData;
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

    PC_CORE::CommandListCreateInfo commandListCreateInfo =
    {
       .commandPoolFamily = PC_CORE::CommandPoolFamily::Graphics,
       .commandBufferType = CommandBufferType::Primary,
    };

    primaryCommandList = Rhi::CreateCommandList(commandListCreateInfo);
    swapChainPassCommandList = Rhi::CreateCommandList(commandListCreateInfo);

   
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
    const auto& rContextView = m_CurrentView->GetRenderingContext();
    const auto& gpuCamera = m_CurrentView->GetCameraGpu();


    uniformBuffers.cameraUniformBuffer.Update(&gpuCamera, sizeof(CameraGpu));
}

void Renderer::UpdateLightGPUData(CommandList* commandlist)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    const auto& rContextView = m_CurrentView->GetRenderingContext();
    const auto& gpuCamera = m_CurrentView->GetCameraGpu();

    int updateDirLight = 0;
    int spotLight = 0;
    int pointLightUpdate = 0;

    for (size_t i = 0; i < renderWorldData.lightData.size(); i++)
    {
        LightData& lightData = renderWorldData.lightData[i];

        switch (lightData.lightType)
        {
        case LightType::Directional:

            if (updateDirLight >= MAX_DIRLIGHT)
                continue;
            {
                DirectionalLightGPU& gpudirLightData = m_GpuDynamicLightData.directionalLights[updateDirLight];
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
                SpotLightGPU& spotLightGPU = m_GpuDynamicLightData.spothLights[spotLight];
                spotLightGPU =
                {
                .position = static_cast<Tbx::Vector3f>(lightData.data.spotLight.position - rContextView.lowLevelCamera.position),
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
            if (pointLightUpdate >= MAX_POINTLIGHT)
                continue;

            {
                const Tbx::Vector3f p3 = static_cast<Tbx::Vector3f>(lightData.data.pointLightData.position - rContextView.lowLevelCamera.position);
                const Tbx::Vector4f p4 = gpuCamera.view * Tbx::Vector4f(p3.x, p3.y, p3.z, 1.0);
                const Tbx::Vector3f p3ViewSpace = Tbx::Vector3f(p4.x, p4.y, p4.z);

                PointLightGPU& pointLightGpu = m_GpuDynamicLightData.pointLights[pointLightUpdate];
                pointLightGpu.position = p3ViewSpace;
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
    m_GpuDynamicLightData.dirLightCount = updateDirLight;
    m_GpuDynamicLightData.spothLightCount = spotLight;
    m_GpuDynamicLightData.pointLightCount = pointLightUpdate;


    constexpr size_t size = sizeof(GPUDynamicLightData);
    gpuLightUniformBufferStaging.Update(&m_GpuDynamicLightData, sizeof(m_GpuDynamicLightData));

    
    commandlist->CopyBuffer(*gpuLightUniformBufferStaging.GetRhiBuffer(),
                               *uniformBuffers.dynamicGpuLightUniformBuffer.GetRhiBuffer(), 0, 0, sizeof(GPUDynamicLightData));

    const BufferMemoryBarrier barrier =
    {
        .srcAccessMask = GpuAccessFlag::TransferWrite,
        .dstAccessMask = GpuAccessFlag::ShaderRead,
        .buffer = uniformBuffers.dynamicGpuLightUniformBuffer.GetRhiBuffer().get(),
        .offset = 0,
        .size = sizeof(GPUDynamicLightData),
    };
    commandlist->Barrier(
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
    const auto& rContextView = m_CurrentView->GetRenderingContext();

    UpdateGpuCameraData();

    Vulkan::VulkanCommandList* vkCommandList = reinterpret_cast<Vulkan::VulkanCommandList*>(primaryCommandList.get());

    primaryCommandList->Reset();
    primaryCommandList->BeginRecordCommands();
#ifdef WITH_EDITOR
    m_DebugDrawContext->Prepare();
#endif
    UpdateLightGPUData(primaryCommandList.get());

    const ViewportInfo viewportInfo =
    {
        .transform = {0, 0},
        .size = {
            static_cast<float>(rContextView.renderingContextSize.x),
            static_cast<float>(rContextView.renderingContextSize.y)
        },
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .scissorsOff = {0, 0},
        .scissorsextent = {rContextView.renderingContextSize.x, rContextView.renderingContextSize.y}
    };

    primaryCommandList->SetViewPort(viewportInfo);
    DefferdPass(viewportInfo);
    ForwardPass(viewportInfo);
    PostProcess(viewportInfo);
    FinalPass(viewportInfo);

    primaryCommandList->EndRecordCommands();
    primaryCommandList->Flush(FlushCommandMethod::Sync
        , GpuPipelineStageFlagBits::ColorAttachmentOutput); // flush

}


void Renderer::SwapBuffers(Window* _window)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    std::shared_ptr<PC_CORE::SwapChain> swapChain = RhiContext::GetContext().swapChain;


    swapChainPassCommandList->Reset();
    swapChainPassCommandList->BeginRecordCommands();

    swapChain->BeginSwapChainRenderPass(swapChainPassCommandList.get());
    swapChainPassCommandList->ExecuteExternalCommand();
    swapChain->EndSwapChainRenderPass(swapChainPassCommandList.get());

    swapChainPassCommandList->EndRecordCommands();
    swapChainPassCommandList->Flush(FlushCommandMethod::Sync, GpuPipelineStageFlagBits::ColorAttachmentOutput);
  

    ClearRenderData();
    m_RhiContext->swapChain->Present(_window);
    Rhi::NextFrame();
}

std::shared_ptr<View> Renderer::CreateView(Tbx::Vector2i _defaultSize)
{
    std::shared_ptr<View> view = std::make_unique<View>(this, _defaultSize);

    m_Views.push_back(view);

    return view;
}


void Renderer::DrawStaticMesh(MaterialType type, std::shared_ptr<PC_CORE::GraphicShader> shader)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    const auto& rContextView = m_CurrentView->GetRenderingContext();

    Tbx::Vector3d cameraOffset = static_cast<Tbx::Vector3d>(rContextView.lowLevelCamera.position);

    for (size_t i = 0; i < renderWorldData.staticMeshData.size(); i++)
    {
        if (!renderWorldData.staticMeshData[i].mesh->IsLoaded())
            continue;

        if (renderWorldData.staticMeshData[i].materialType != type)
            continue;

        Tbx::Matrix4x4f modelMatrixf[2];
        // Apply offset to the copy one
        modelMatrixf[0] = renderWorldData.staticMeshData[i].worldMatrix;
        modelMatrixf[0][12] -= cameraOffset.x;
        modelMatrixf[0][13] -= cameraOffset.y;
        modelMatrixf[0][14] -= cameraOffset.z;

        modelMatrixf[1] = renderWorldData.staticMeshData[i].normalInvertMatrix;


        const ShaderProgramDescriptorSets* materialDescriptor = renderWorldData.staticMeshData[i].descriptorSet;
        const Mesh* mesh = renderWorldData.staticMeshData[i].mesh;

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

    renderWorldData.lightData.clear();
    renderWorldData.staticMeshData.clear();
}


void Renderer::DrawSkyBox()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    if (auto cube = m_CubeMesh.lock())
    {
        primaryCommandList->BindProgram(m_CubeMapShader.lock().get());
        primaryCommandList->BindDescriptorSet(m_CubeMapShader.lock().get(), skyboxCameraDescriptorSet,
            SCENE_DESCRIPTOR_SET, 1);
        primaryCommandList->BindDescriptorSet(m_CubeMapShader.lock().get(), skyBoxCubeMapDescriptorSet,
            ENVIRONEMENT_DESCRIPTOR_SET, 1);
        primaryCommandList->BindVertexBuffer(*cube->vertexBuffer.GetRhiBuffer(), 0, 1);
        primaryCommandList->BindIndexBuffer(*cube->indexBuffer.GetRhiBuffer(), 0);
        primaryCommandList->DrawIndexed(cube->indexBuffer.GetIndexCount(), 1, 0, 0, 0);
    }
}


void Renderer::ForwardPass(const ViewportInfo& _viewportInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    const auto& rContextView = m_CurrentView->GetRenderingContext();


    const BeginRenderPassInfo beginRenderPassInfo =
    {
        .renderPass = renderPasses.forwardPass,
        .frameBuffer = rContextView.forwardFrameBuffer,
        .renderOffSet = {0, 0},
        .extent = {rContextView.renderingContextSize.x, rContextView.renderingContextSize.y},
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
    primaryCommandList->BindDescriptorSet(m_ForwardShader.lock().get(), rContextView.forwardDesritptorSet,
        SCENE_DESCRIPTOR_SET, 1);

    // draw all static mesh
    DrawStaticMesh(MaterialType::Transparent, m_ForwardShader.lock());
    DrawSkyBox();
#ifdef WITH_EDITOR
    m_DebugDrawContext->DrawDebugPrimitive(primaryCommandList.get(), rContextView);
    for (auto& it : UserCustomForwardPass)
        it(*this, primaryCommandList.get(), rContextView, &renderWorldData);
#endif
    primaryCommandList->EndRenderPass();

    primaryCommandList->EndDebugLabel();
}

void Renderer::DefferdPass(const ViewportInfo& _viewportInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    const auto& rContextView = m_CurrentView->GetRenderingContext();

    ClearValueFlags clearValueFlags = static_cast<ClearValueFlags>(ClearValueFlags::ClearValueColor |
        ClearValueFlags::ClearValueDepth);

    // + 1 for depth 
    std::array<Tbx::Vector4f, static_cast<uint8_t>(GbufferType::Count) + 1> clearValues2 = {
        Tbx::Vector4f(0, 1, 0, 1.f),
    };

    const BeginRenderPassInfo beginRenderPassInfo =
    {
        .renderPass = renderPasses.defferedPass,
        .frameBuffer = rContextView.gbufferFrameBuffer,
        .renderOffSet = {0, 0},
        .extent = {rContextView.renderingContextSize.x, rContextView.renderingContextSize.y},
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

        primaryCommandList->BindDescriptorSet(sGeometry.get(), rContextView.geometryDescritproSet, SCENE_DESCRIPTOR_SET, 1);
        DrawStaticMesh(MaterialType::Opaque, sGeometry);
    }
    primaryCommandList->EndDebugLabel();

    primaryCommandList->NextSubPass();

    primaryCommandList->BeginDebugLabel("DeferredPass", DEFERD_PASS_COLOR);

    if (std::shared_ptr sDeferred = m_DeferedShader.lock())
    {
        primaryCommandList->BindProgram(sDeferred.get());
        primaryCommandList->BindDescriptorSet(sDeferred.get(), rContextView.defferdLightingLightingCameraSet, SCENE_DESCRIPTOR_SET, 1);
        primaryCommandList->BindDescriptorSet(sDeferred.get(), rContextView.defferdLightingGbufferSet, GBUFFER_SET, 1);

        primaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleStrip);
        primaryCommandList->Draw(4, 1, 0, 0);
    }

    primaryCommandList->EndDebugLabel();

    primaryCommandList->EndRenderPass();
}

PC_CORE_API void Renderer::PostProcess(const ViewportInfo& _viewportInfo)
{
    const auto& rContextView = m_CurrentView->GetRenderingContext();


    primaryCommandList->BeginDebugLabel("PostProcess Pass", POST_PROCESS);

    if (auto aces = m_AcesShader.lock().get())
    {
        primaryCommandList->BindProgram(aces);
        primaryCommandList->BindDescriptorSet(aces, rContextView.toneMapDescritptorSet, 0, 1);

        const LocalSize& localSize = aces->GetLocalSize();

        uint32_t groupX = ((uint32_t)_viewportInfo.size.x + localSize.x - 1) / localSize.x;
        uint32_t groupY = (uint32_t)_viewportInfo.size.y;


        const ImageMemoryBarrier gbufferImage =
        {
            .srcAccessMask = GpuAccessFlag::ColorAttachmentWrite,
            .dstAccessMask = static_cast<GpuAccessFlag>(GpuAccessFlag::ShaderRead & GpuAccessFlag::ShaderWrite),

            .currentState = ImageState::RenderTargetOptimal,
            .newState = ImageState::General,

            .texture = rContextView.hdrImage->GetRhiTexture2D().get(),
        };

        primaryCommandList->Barrier(GpuPipelineStageFlagBits::ColorAttachmentOutput,
            GpuPipelineStageFlagBits::ComputeShader,
            nullptr, 0,
            nullptr, 0,
            &gbufferImage, 1);

        primaryCommandList->Dispatch(groupX, groupY, 1);

        assert(rContextView.hdrImage != nullptr);

    }
    primaryCommandList->EndDebugLabel();
}


void Renderer::FinalPass(const ViewportInfo& _viewportInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    std::array<Tbx::Vector4f, 2> clearValues2 = {
        Tbx::Vector4f(0, 0, 0, 0.f),
        Tbx::Vector4f(0, 0, 0, 0.f),
    };

    const auto& rContextView = m_CurrentView->GetRenderingContext();

    const BeginRenderPassInfo drawToViewport =
    {
        .renderPass = renderPasses.drawToFinalViewPort,
        .frameBuffer = rContextView.finalImageFrameBuffer,
        .renderOffSet = {0, 0},
        .extent = {rContextView.renderingContextSize.x, rContextView.renderingContextSize.y},
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
        rContextView.finalImageDescritptorSet, 0, 1);
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
        // + 1 lit image
        attachements.resize(
            static_cast<std::vector<RenderPassAttachementDescriptor>::size_type>(GbufferType::Count) + 1);

        attachements[static_cast<uint8_t>(GbufferType::Albedo)] =
        {
            .attachmentType = AttachmentType::Color,
            .format = PC_CORE::RHIFormat::R16G16B16A16_SFLOAT,
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
            .format = PC_CORE::RHIFormat::R16G16_SNORM,
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
            .format = PC_CORE::RHIFormat::R8G8B8A8_UNORM,
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
            .format = PC_CORE::RHIFormat::R16G16B16A16_SFLOAT,
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
            .format = PC_CORE::RHIFormat::D32_SFLOAT,
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
            .format = PC_CORE::RHIFormat::R16G16B16A16_SFLOAT,
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
            .format = PC_CORE::RHIFormat::R16G16B16A16_SFLOAT,
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
            .format = PC_CORE::RHIFormat::D32_SFLOAT,
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

    {
        PERF_REGION_SCOPED_NAMED("ToneMap Shader");

        const ComputeShaderProgramCreateInfo computeShaderProgramCreateInfo =
            {
            .shaderComputeInfo = {},
            .source = ResourceManager::Get<ShaderSourceBinary>("aces_spv.comp")
            };
        
        m_AcesShader = ResourceManager::Create<PC_CORE::ComputeShader>("Aces", computeShaderProgramCreateInfo);
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
        m_CubeMesh = ResourceManager::Get<Mesh>("cube.obj");
    }
}
void Renderer::CreateBuffers()
{
    uniformBuffers.cameraUniformBuffer = UniformBuffer(sizeof(CameraGpu),
        PC_CORE::MemoryLocalisation::CPU_To_GPU, MemoryUsage::Dynamic);
    uniformBuffers.postProcessUniformBuffer = UniformBuffer(&uniformBuffers.postProcessUniformBuffer, sizeof(PostProcessGpu),
        PC_CORE::MemoryLocalisation::CPU_To_GPU, MemoryUsage::Dynamic);

    gpuLightUniformBufferStaging = UniformBuffer(&uniformBuffers.postProcessUniformBuffer, sizeof(m_GpuDynamicLightData),
        PC_CORE::MemoryLocalisation::CPU_To_GPU, MemoryUsage::Dynamic);

    uniformBuffers.dynamicGpuLightUniformBuffer = UniformBuffer(&m_GpuDynamicLightData, sizeof(m_GpuDynamicLightData),
        PC_CORE::MemoryLocalisation::GPU_Only, MemoryUsage::Dynamic);
}
#pragma endregion CreateThirdPartyResources

#pragma region CreateDescriptorSets

void Renderer::CreateDescriptorSets()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    /////////////////////////////////////////////////
   
    const UniformBufferDescriptor lightData
    {
        .buffer = &uniformBuffers.cameraUniformBuffer,
    };

    ImageSamplerDescriptor skyboxCubeMapDescritptor
    {
        .sampler = ResourceManager::Get<Sampler>("LinearRepeat").get(),
        .texture = m_Cubemap.lock().get(),
        .imageState = PC_CORE::ImageState::ShaderReadOptimal
    };

    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets;

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
        m_CubeMapShader.lock()->AllocDescriptorSet(&skyboxCameraDescriptorSet, SCENE_DESCRIPTOR_SET);
        skyboxCameraDescriptorSet->WriteDescriptorSets(descriptorSets);

        descriptorSets =
        {
            {
                ShaderProgramDescriptorType::CombinedImageSampler,
                SKYBOX_BINDING,
                skyboxCubeMapDescritptor,
            }
        };
        m_CubeMapShader.lock()->AllocDescriptorSet(&skyBoxCubeMapDescriptorSet,
                                                   ENVIRONEMENT_DESCRIPTOR_SET);
        skyBoxCubeMapDescriptorSet->WriteDescriptorSets(descriptorSets);
    }
}
#pragma endregion CreateDescriptorSets
