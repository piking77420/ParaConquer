#include "Rendering/View.hpp"

#include "Rendering/Renderer.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Time/CoreTime.hpp"
#include "Resources/ResourceManager.hpp"

using namespace PC_CORE;

void View::SetRenderingContextFlag(size_t _flag)
{
    RenderingContext.RenderingContextFlag = _flag;
}

void View::SetCamera(Camera* _camera)
{
    m_Camera = _camera;
}

void View::Update()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    UpdateRenderingContext();

    CameraGpu.Time = RenderingContext.Time;
    CameraGpu.Deltatime = RenderingContext.DeltaTime;

    const Tbx::Matrix4x4f view = Tbx::LookAtRH<float>(Tbx::Vector3f::Zero(),
                                                      RenderingContext.LowLevelCamera.Front,
                                                      RenderingContext.LowLevelCamera.Up);

    const Tbx::Matrix4x4f projection = Tbx::PerspectiveMatrixFlipYAxis<float>(RenderingContext.LowLevelCamera.Fov,
                                                                              RenderingContext.LowLevelCamera.Aspect,
                                                                              RenderingContext.LowLevelCamera.Near,
                                                                              RenderingContext.LowLevelCamera.Far);

    CameraGpu.Vp = projection * view;
    CameraGpu.View = view;
    CameraGpu.Proj = projection;
    CameraGpu.ViewInv = CameraGpu.View.Invert();
    CameraGpu.ProjInv = CameraGpu.Proj.Invert();
    CameraGpu.VpInv = CameraGpu.Vp.Invert();
    CameraGpu.CameraNear = RenderingContext.LowLevelCamera.Near;
    CameraGpu.CameraFar = RenderingContext.LowLevelCamera.Far;
    CameraGpu.CameraPos = static_cast<Tbx::Vector3f>(RenderingContext.LowLevelCamera.Position);


    PostProcessGpu.Gamma = 2.2;
    PostProcessGpu.Exposure = 1;
}


void View::Resize(Tbx::Vector2i _viewPortSize)
{
    PC_LOG_VERBOSE("Resize View");
    m_CurrentSize = _viewPortSize;
    m_Renderer->GetRhi().GetRhiContext().WaitIdle(); // this func should be remove when deferred destroy will be implemented

    CreateImages();
    CreateFrameBuffers();
    CreateDescritproSets();
}

void View::UpdateRenderingContext()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    if (m_Camera == nullptr)
        return;

    // Camera
    RenderingContext.LowLevelCamera =
    {
        .Position = m_Camera->Position,
        .Front = static_cast<Tbx::Vector3f>(m_Camera->Front),
        .Up = static_cast<Tbx::Vector3f>(m_Camera->Up),
        .Aspect = m_Camera->GetAspect(),
        .Fov = m_Camera->GetFov(),
        .Near = m_Camera->GetNear(),
        .Far = m_Camera->GetFar(),
        .IsOrthographic = m_Camera->GetProjectionType() == ProjectionType::Orthographic,
    };
    RenderingContext.Time = Time::GetTime();
    RenderingContext.DeltaTime = Time::DeltaTime();

    // Frame Buffer
    RenderingContext.ForwardFrameBuffer = m_FrameBuffers.ForwardFrameBuffer;
    RenderingContext.GbufferFrameBuffer = m_FrameBuffers.GbufferFrameBuffer;
    RenderingContext.FinalImageFrameBuffer = m_FrameBuffers.FinalImageFrameBuffer;

    // Descriptor
    RenderingContext.GeometryDescritproSet = m_DescriptorSets.GeometryPass.get();
    RenderingContext.DefferdLightingGbufferSet = m_DescriptorSets.DefferedPassGbuffers.get();
    RenderingContext.DefferdLightingLightingCameraSet = m_DescriptorSets.DefferedPassCameraLight.get();
    RenderingContext.ForwardDesritptorSet = m_DescriptorSets.ForwardDescriptor.get();
    RenderingContext.ToneMapDescritptorSet = m_DescriptorSets.ToneMap.get();
    RenderingContext.FinalImageDescritptorSet = m_DescriptorSets.FinalViewPort.get();

    RenderingContext.HdrImage = ForwardTexture.color.get();
    RenderingContext.RenderingContextSize = {
        static_cast<uint32_t>(m_CurrentSize.x), static_cast<uint32_t>(m_CurrentSize.y)
    };
}

void View::CreateImages()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    Rhi& Rhi = m_Renderer->GetRhi();

    const uint32_t Width = static_cast<uint32_t>(m_CurrentSize.x);
    const uint32_t Height = static_cast<uint32_t>(m_CurrentSize.y);

    Gbuffers.CreateGBuffers(Rhi, m_CurrentSize);

    // Forward
    {
        ForwardTexture.color
            ->SetWidth(Width)
            .SetHeight(Height)
            .SetTextureUsage(RhiTexture::RenderTarget | RhiTexture::Sampled | RhiTexture::LoadAndStore)
            .SetRhiFormat(RhiFormat::R16G16B16A16Sfloat)
            .SetName("View Forward Texture")
            .Build();

        ForwardTexture.depth
            ->SetWidth(Width)
            .SetHeight(Height)
            .SetTextureUsage(RhiTexture::DepthStencil)
            .SetRhiFormat(RhiFormat::D24UnormS8Uint)
            .SetName("View Forward Texture")
            .Build();
    }


    // Final Image
    {
        FinalImage
            ->SetWidth(Width)
            .SetHeight(Height)
            .SetTextureUsage(RhiTexture::RenderTarget | RhiTexture::Sampled)
            .SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
            .SetName("View Forward Texture")
            .Build();

        ResolvedImages
            ->SetWidth(Width)
            .SetHeight(Height)
            .SetTextureUsage(RhiTexture::RenderTarget | RhiTexture::Sampled)
            .SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
            .SetSamples(Rhi.GetRhiContext().rhiPhysicalDevices->GetPhysicalDevice().GetMaxUsableSampleCount())
            .SetName("View Forward Texture")
            .Build();
    }
}

void View::CreateFrameBuffers()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    Rhi& Rhi = m_Renderer->GetRhi();
    const uint32_t Width = static_cast<uint32_t>(m_CurrentSize.x);
    const uint32_t Height = static_cast<uint32_t>(m_CurrentSize.y);

    // Gbuffer FrameBuffer
    {
        /*
        std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;

        constexpr size_t DepthCount = 1;
        constexpr size_t LitImageCount = 1;

        attechementDescriptor.resize(static_cast<size_t>(GbufferType::Count) + DepthCount + LitImageCount);

        size_t i = 0;
        for (; i < static_cast<size_t>(GbufferType::Count); i++)
            attechementDescriptor[i].RhiTexture = Gbuffers.gbuffers[i].Get();


        attechementDescriptor[i++].RhiTexture = ForwardTexture.color.Get();
        attechementDescriptor[i++].RhiTexture = ForwardTexture.depth.Get();

        assert(ForwardTexture.depth.Get() == attechementDescriptor[attechementDescriptor.size() - 1].RhiTexture &&
            "Backend expect thaht depth is the last attachement");

        createFrameBufferInfo.Attachements = &attechementDescriptor;
        createFrameBufferInfo.RenderPass = m_Renderer->RenderPasses.DefferedPass.get();

        // HANDLE INPUT ATTACHEMENT
        //m_FrameBuffers.GbufferFrameBuffer.reset(Rhi::CreateFrameBuffer("Gbuffer FrameBuffer", createFrameBufferInfo));*/
    }

    // Forward FrameBuffer
    {

        m_FrameBuffers.ForwardFrameBuffer.reset(Rhi.CreateFrameBuffer()) ;
        m_FrameBuffers.ForwardFrameBuffer
            ->SetWidth(Width)
            .SetHeight(Height)
            .SetAttachments(
                ForwardTexture.color.get(),
                ForwardTexture.depth.get())
            .SetRenderPass(m_Renderer->RenderPasses.ForwardPass.get())
            .SetName("ForwardFrameBuffer")
            .Build();
    }


    // Final Image
    {
        m_FrameBuffers.FinalImageFrameBuffer
            ->SetWidth(Width)
            .SetHeight(Height)
            .SetAttachments(
                ResolvedImages.get(),
                FinalImage.get())
            .SetRenderPass(m_Renderer->RenderPasses.DrawToFinalViewPort.get())
            .SetName("FinalImageFrameBuffer")
            .Build();
    }
}

void View::CreateDescritproSets()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    const BufferDescriptor cameraBufferDescritptor
    {
        .buffer = m_Renderer->UniformBuffers.Camera.get(),
    };

    const BufferDescriptor lightData
    {
        .buffer = m_Renderer->UniformBuffers.LightBuffer.get(),
    };

    const ImageSamplerDescriptor skyboxCubeMapDescritptor
    {
        .sampler = m_Renderer->LinearReapeat.Get(),
        .texture = m_Renderer->Cubemap.Lock()->Get(),
        .resourceState = RhiResourceState::ShaderRead
    };

    std::vector<DescriptorWrite> descriptorWrites;


    {
        /*
        PERF_REGION_SCOPED_NAMED("Create geometryPass DescriptorSets");
        std::shared_ptr<GraphicShader> geometryShaderPass = m_Renderer->GeometryBufferShader.lock();
        descriptorWrites =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                cameraBufferDescritptor,
            }
        };

        if (m_DescriptorSets.GeometryPass != nullptr)
            geometryShaderPass->FreeDescriptorSet(&m_DescriptorSets.GeometryPass);

        geometryShaderPass->CreateDescriptorBinding(&m_DescriptorSets.GeometryPass, SCENE_DESCRIPTOR_SET);
        m_DescriptorSets.GeometryPass->SetBindings(descriptorWrites);*/
    }

    // Defferd Lighting Descritptors
    {
        /*
        PERF_REGION_SCOPED_NAMED("Create Defferd DescriptorSets");
        // Gbuffers
        std::shared_ptr<GraphicShader> deferredShader = m_Renderer->DeferedShader.lock();
        descriptorWrites.resize(static_cast<uint8_t>(GbufferType::Count));

        std::array<InputAttachementDescriptor, static_cast<uint8_t>(GbufferType::Count)> inputAttachements;

        for (size_t i = 0; i < static_cast<uint8_t>(GbufferType::Count); i++)
        {
            inputAttachements[i] =
                {
                    .image = Gbuffers.gbuffers[i].Get(),
                    .resourceState = RhiResourceState::ShaderRead
                },

                descriptorWrites[i] =
                {
                    .shaderProgramDescriptorType = ShaderProgramDescriptorType::InputAttachment,
                    .bindingIndex = static_cast<uint32_t>(i),
                    .descriptor = inputAttachements[i]
                };
        }

        if (m_DescriptorSets.DefferedPassGbuffers != nullptr)
            deferredShader->FreeDescriptorSet(&m_DescriptorSets.DefferedPassGbuffers);

        deferredShader->CreateDescriptorBinding(&m_DescriptorSets.DefferedPassGbuffers, GBUFFER_SET);
        m_DescriptorSets.DefferedPassGbuffers->SetBindings(descriptorWrites);


        descriptorWrites.resize(2);
        // Camera And Lighting
        descriptorWrites =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                0,
                lightData,
            }
        };

        if (m_DescriptorSets.DefferedPassCameraLight != nullptr)
            deferredShader->FreeDescriptorSet(&m_DescriptorSets.DefferedPassCameraLight);

        deferredShader->CreateDescriptorBinding(&m_DescriptorSets.DefferedPassCameraLight, SCENE_DESCRIPTOR_SET);
        m_DescriptorSets.DefferedPassCameraLight->SetBindings(descriptorWrites);*/
    }

    {
        
        PERF_REGION_SCOPED_NAMED("Create Forward Shader DescriptorSet");
        RhiShaderProgram* forwardShader = m_Renderer->ForwardShader.get();

        descriptorWrites =
        {

            {
                DescriptorType::UniformBuffer,
                CAMERA_BINDING,
                cameraBufferDescritptor,
            }/*,
            {
                DescriptorType::UniformBuffer,
                LIGHTDATA_BINDING,
                lightData,
            },
            {
                DescriptorType::CombinedImageSampler,
                FORWARD_SKYBOX_CUBEMAP,
                skyboxCubeMapDescritptor,
            }*/
        };


        m_DescriptorSets.ForwardDescriptor.reset(forwardShader->CreateDescriptorBinding());
        m_DescriptorSets.ForwardDescriptor
            ->SetBindings(SCENE_DESCRIPTOR_SET, descriptorWrites)
            .SetName("View Forward Descriptor")
            .Build();
    }


    {
        PERF_REGION_SCOPED_NAMED("Create ToneMap DescriptorSet");

        RhiShaderProgram* ToneMappShader = m_Renderer->AcesShader.get();

        struct ImageDescriptor hdrImage
        {
            .texture = ForwardTexture.color.get(),
            .resourceState = RhiResourceState::ComputeWrite,
        };


        descriptorWrites.resize(1);
        descriptorWrites =
        {

            {
                DescriptorType::StorageImage,
                0,
                hdrImage,
            },
        };

        m_DescriptorSets.ToneMap.reset(ToneMappShader->CreateDescriptorBinding());
        m_DescriptorSets.ToneMap
            ->SetBindings(0, descriptorWrites)
            .SetName("Tome map view binging")
            .Build();
    }

    {
        PERF_REGION_SCOPED_NAMED("FinalViewPort DescriptorSet");
        RhiShaderProgram* drawToFinalImage = m_Renderer->DrawTextureScreenQuadShader.get();

        const ImageSamplerDescriptor finalImageDescrotproSet
        {
            .sampler = m_Renderer->LinearReapeat.Get(),
            .texture = ForwardTexture.color.get(),
            .resourceState = RhiResourceState::ShaderRead
        };

        descriptorWrites.resize(1);
        descriptorWrites =
        {

            {
                DescriptorType::CombinedImageSampler,
                0,
                finalImageDescrotproSet,
            }
        };

        m_DescriptorSets.FinalViewPort.reset(drawToFinalImage->CreateDescriptorBinding());
        m_DescriptorSets.FinalViewPort
            ->SetBindings(0, descriptorWrites)
            .SetName("drawToFinalImage View Binding")
            .Build();
    }
}


View::View(Renderer* _renderer, Tbx::Vector2i _viewPortSize) : m_Renderer(_renderer)
{
    Resize(_viewPortSize);
}
