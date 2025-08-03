#include "rendering/view.hpp"

#include "rendering/renderer.hpp"
#include "low_renderer/rhi.hpp"
#include "time/core_time.hpp"
#include "resources/resource_manager.hpp"

using namespace PC_CORE;

void View::SetRenderingContextFlag(size_t _flag)
{
	m_RenderingContext.renderingContextFlag = _flag;
}

void View::SetCamera(PC_CORE::Camera* _camera)
{
	m_Camera = _camera;
}

void View::Update()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Rendering);
	UpdateRenderingContext();

	m_CameraGpu.time = m_RenderingContext.time;
	m_CameraGpu.deltatime = m_RenderingContext.deltaTime;

	const Tbx::Matrix4x4f view = Tbx::LookAtRH<float>(Tbx::Vector3f::Zero(),
		m_RenderingContext.lowLevelCamera.front,
		m_RenderingContext.lowLevelCamera.up);

	const Tbx::Matrix4x4f projection = Tbx::PerspectiveMatrixFlipYAxis<float>(m_RenderingContext.lowLevelCamera.fov,
		m_RenderingContext.lowLevelCamera.aspect,
		m_RenderingContext.lowLevelCamera.near,
		m_RenderingContext.lowLevelCamera.far);

	m_CameraGpu.vp = projection * view;
	m_CameraGpu.view = view;
	m_CameraGpu.proj = projection;
	m_CameraGpu.viewInv = m_CameraGpu.view.Invert();
	m_CameraGpu.projInv = m_CameraGpu.proj.Invert();
	m_CameraGpu.vpInv = m_CameraGpu.vp.Invert();
	m_CameraGpu.cameraNear = m_RenderingContext.lowLevelCamera.near;
	m_CameraGpu.cameraFar = m_RenderingContext.lowLevelCamera.far;
	m_CameraGpu.cameraPos = static_cast<Tbx::Vector3f>(m_RenderingContext.lowLevelCamera.position);


	m_PostProcessGpu.gamma = 2.2;
	m_PostProcessGpu.exposure = 1;
}


void View::Resize(Tbx::Vector2i _viewPortSize)
{
	PC_LOG_VERBOSE("Resize View");
	m_CurrentSize = _viewPortSize;
	PC_CORE::Rhi::GetRhiContext()->WaitIdle();

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
	m_RenderingContext.lowLevelCamera =
	{
		.position = m_Camera->position,
		.front = static_cast<Tbx::Vector3f>(m_Camera->front),
		.up = static_cast<Tbx::Vector3f>(m_Camera->up),
		.aspect = m_Camera->GetAspect(),
		.fov = m_Camera->GetFOV(),
		.near = m_Camera->GetNear(),
		.far = m_Camera->GetFar(),
		.isOrthographic = m_Camera->GetProjectionType() == PC_CORE::ProjectionType::ORTHOGRAPHIC,
	};
	m_RenderingContext.time = PC_CORE::Time::GetTime();
	m_RenderingContext.deltaTime = PC_CORE::Time::DeltaTime();

	// Frame Buffer
	m_RenderingContext.forwardFrameBuffer = m_FrameBuffers.forwardFrameBuffer;
	m_RenderingContext.gbufferFrameBuffer = m_FrameBuffers.gbufferFrameBuffer;
	m_RenderingContext.finalImageFrameBuffer = m_FrameBuffers.finalImageFrameBuffer;

	// Descriptor
	m_RenderingContext.geometryDescritproSet = m_DescriptorSets.geometryPass;
	m_RenderingContext.defferdLightingGbufferSet = m_DescriptorSets.defferedPassGbuffers;
	m_RenderingContext.defferdLightingLightingCameraSet = m_DescriptorSets.defferedPassCameraLight;
	m_RenderingContext.forwardDesritptorSet = m_DescriptorSets.forwardDescriptor;
	m_RenderingContext.toneMapDescritptorSet = m_DescriptorSets.toneMap;
	m_RenderingContext.finalImageDescritptorSet = m_DescriptorSets.finalViewPort;

	m_RenderingContext.hdrImage = &m_ForwardTexture.color;
	m_RenderingContext.renderingContextSize = { static_cast<uint32_t>(m_CurrentSize.x), static_cast<uint32_t>(m_CurrentSize.y) };
}

void View::CreateImages()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Rendering);

	m_Gbuffers.CreateGBuffers(m_CurrentSize);

	// Forward
	{
		PC_CORE::CreateImageInfo createInfo =
		{
			 .width = m_CurrentSize.x,
			 .height = m_CurrentSize.y,
			 .depth = 1,
			 .layerCount = 1,
			 .mipsLevels = 1,
			 .textureType = PC_CORE::TextureType::Texture2D,
			 .format = PC_CORE::RHIFormat::R16G16B16A16_SFLOAT,
			 .channel = PC_CORE::Channel::RGBA,
			 .textureUsage = PC_CORE::TextureUsage::RenderTarget | PC_CORE::TextureUsage::Sampled | PC_CORE::TextureUsage::Storage,
			 .memoryVisibility = PC_CORE::MemoryLocalisation::GPU_Only,
			 .samples = 1,
			 .GenerateMipMap = false,
			 .datas = {}
		};

		m_ForwardTexture.color = Texture2D(createInfo);

		createInfo.format = RHIFormat::D32_SFLOAT;
		createInfo.channel = Channel::GREY;
		createInfo.textureUsage = PC_CORE::TextureUsage::Depth;

		m_ForwardTexture.depth = Texture2D(createInfo);
	}

	// Final Image
	{
		PC_CORE::CreateImageInfo createInfo =
		{
		     .width = m_CurrentSize.x,
			 .height = m_CurrentSize.y,
			 .depth = 1,
			 .layerCount = 1,
			 .mipsLevels = 1,
			 .textureType = PC_CORE::TextureType::Texture2D,
			 .format = PC_CORE::RHIFormat::R8G8B8A8_UNORM,
			 .channel = PC_CORE::Channel::RGBA,
			 .textureUsage = PC_CORE::TextureUsage::RenderTarget | PC_CORE::TextureUsage::Sampled,
			 .memoryVisibility = PC_CORE::MemoryLocalisation::GPU_Only,
			 .samples = 1,
			 .GenerateMipMap = false,
			 .datas = {}
		};

		  m_FinalImage = PC_CORE::Texture2D(createInfo);
		  createInfo.samples = PC_CORE::Rhi::GetRhiContext()->physicalDevices->GetPhysicalDevice().GetMaxUsableSampleCount();
		  m_ResolvedImages = PC_CORE::Texture2D(createInfo);

	}
}

void View::CreateFrameBuffers()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Rendering);

	CreateFrameInfo createFrameBufferInfo =
	{
	.width = static_cast<uint32_t>(m_CurrentSize.x),
	.height = static_cast<uint32_t>(m_CurrentSize.y),
	};


	// Gbuffer FrameBuffer
	{
		std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;

		constexpr size_t DepthCount = 1;
		constexpr size_t LitImageCount = 1;

		attechementDescriptor.resize((size_t)GbufferType::Count + DepthCount + LitImageCount);

		size_t i = 0;
		for (; i < (size_t)GbufferType::Count; i++)
			attechementDescriptor[i].texture = &m_Gbuffers.gbuffers[i];


		attechementDescriptor[i++].texture = &m_ForwardTexture.color;
		attechementDescriptor[i++].texture = &m_ForwardTexture.depth;

		assert(&m_ForwardTexture.depth == attechementDescriptor[attechementDescriptor.size() - 1].texture &&
			"Backend expect thaht depth is the last attachement");

		createFrameBufferInfo.attachements = &attechementDescriptor;
		createFrameBufferInfo.renderPass = m_Renderer->renderPasses.defferedPass.get();


		m_FrameBuffers.gbufferFrameBuffer = Rhi::CreateFrameBuffer(createFrameBufferInfo);
	}

	// Forward FrameBuffer
	{
		std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;
		attechementDescriptor.resize(2); // color + depth;
		attechementDescriptor[0].texture = &m_ForwardTexture.color;
		attechementDescriptor[1].texture = &m_ForwardTexture.depth;


		createFrameBufferInfo.attachements = &attechementDescriptor;
		createFrameBufferInfo.renderPass = m_Renderer->renderPasses.forwardPass.get();

		m_FrameBuffers.forwardFrameBuffer = Rhi::CreateFrameBuffer(createFrameBufferInfo);
	}

	// Final Image
	{
		std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;
		attechementDescriptor.resize(2); // color + depth;

		attechementDescriptor[0].texture = &m_ResolvedImages;
		attechementDescriptor[1].texture = &m_FinalImage;

		createFrameBufferInfo.attachements = &attechementDescriptor;
		createFrameBufferInfo.renderPass = m_Renderer->renderPasses.drawToFinalViewPort.get();

		m_FrameBuffers.finalImageFrameBuffer = Rhi::CreateFrameBuffer(createFrameBufferInfo);
	}
	
}

void View::CreateDescritproSets()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Rendering);

	std::shared_ptr<PC_CORE::Sampler> sampler = PC_CORE::ResourceManager::Get<PC_CORE::Sampler>("LinearRepeat");


	const UniformBufferDescriptor cameraBufferDescritptor
	{
		.buffer = &m_Renderer->uniformBuffers.cameraUniformBuffer,
	};

	const UniformBufferDescriptor lightData
	{
		.buffer = &m_Renderer->uniformBuffers.dynamicGpuLightUniformBuffer,
	};

	const ImageSamplerDescriptor skyboxCubeMapDescritptor
	{
		.sampler = sampler.get(),
		.texture = m_Renderer->m_Cubemap.lock().get(),
		.imageState = PC_CORE::ImageState::ShaderReadOptimal
	};

	std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorWrites;


	{
		PERF_REGION_SCOPED_NAMED("Create geometryPass DescriptorSets");
		std::shared_ptr<PC_CORE::GraphicShader> geometryShaderPass = m_Renderer->m_GeometryBufferShader.lock();
		descriptorWrites =
		{
			{
				ShaderProgramDescriptorType::UniformBuffer,
				CAMERA_BINDING,
				cameraBufferDescritptor,
			}
		};

		if (m_DescriptorSets.geometryPass != nullptr)
			geometryShaderPass->FreeDescriptorSet(&m_DescriptorSets.geometryPass);

		geometryShaderPass->AllocDescriptorSet(&m_DescriptorSets.geometryPass, SCENE_DESCRIPTOR_SET);
		m_DescriptorSets.geometryPass->WriteDescriptorSets(descriptorWrites);
	}

	// Defferd Lighting Descritptors
	{
		PERF_REGION_SCOPED_NAMED("Create Defferd DescriptorSets");
		// Gbuffers
		std::shared_ptr<GraphicShader> deferredShader = m_Renderer->m_DeferedShader.lock();
		descriptorWrites.resize(static_cast<uint8_t>(GbufferType::Count));

		std::array<InputAttachementDescriptor, static_cast<uint8_t>(GbufferType::Count)> inputAttachements;

		for (size_t i = 0; i < static_cast<uint8_t>(GbufferType::Count); i++)
		{
			inputAttachements[i] =
			{
			.image = &m_Gbuffers.gbuffers[i],
			.imageState = PC_CORE::ImageState::ShaderReadOptimal
			},

				descriptorWrites[i] =
			{
			.shaderProgramDescriptorType = ShaderProgramDescriptorType::InputAttachment,
			.bindingIndex = static_cast<uint32_t>(i),
			.descriptor = inputAttachements[i]
			};
		}

		if (m_DescriptorSets.defferedPassGbuffers != nullptr)
			deferredShader->FreeDescriptorSet(&m_DescriptorSets.defferedPassGbuffers);

		deferredShader->AllocDescriptorSet(&m_DescriptorSets.defferedPassGbuffers, GBUFFER_SET);
		m_DescriptorSets.defferedPassGbuffers->WriteDescriptorSets(descriptorWrites);


		descriptorWrites.resize(2);
		// Camera And Lighting
		descriptorWrites =
		{
			{
				ShaderProgramDescriptorType::UniformBuffer,
				CAMERA_BINDING,
				cameraBufferDescritptor,
			},
			{
				ShaderProgramDescriptorType::UniformBuffer,
				LIGHTDATA_BINDING,
				lightData,
			}
		};

		if (m_DescriptorSets.defferedPassCameraLight != nullptr)
			deferredShader->FreeDescriptorSet(&m_DescriptorSets.defferedPassCameraLight);

		deferredShader->AllocDescriptorSet(&m_DescriptorSets.defferedPassCameraLight, SCENE_DESCRIPTOR_SET);
		m_DescriptorSets.defferedPassCameraLight->WriteDescriptorSets(descriptorWrites);
	}

	{
		PERF_REGION_SCOPED_NAMED("Create Forward Shader DescriptorSet");
		std::shared_ptr<GraphicShader> m_ForwardShader = m_Renderer->m_ForwardShader.lock();

		descriptorWrites.resize(3);

		descriptorWrites =
		{

			{
				ShaderProgramDescriptorType::UniformBuffer,
				CAMERA_BINDING,
				cameraBufferDescritptor,
			},
			{
				ShaderProgramDescriptorType::UniformBuffer,
				LIGHTDATA_BINDING,
				lightData,
			},
			{
				ShaderProgramDescriptorType::CombinedImageSampler,
				FORWARD_SKYBOX_CUBEMAP,
				skyboxCubeMapDescritptor,
			}
		};


		if (m_DescriptorSets.forwardDescriptor != nullptr)
			m_ForwardShader->FreeDescriptorSet(&m_DescriptorSets.forwardDescriptor);

		m_ForwardShader->AllocDescriptorSet(&m_DescriptorSets.forwardDescriptor, SCENE_DESCRIPTOR_SET);
		m_DescriptorSets.forwardDescriptor->WriteDescriptorSets(descriptorWrites);
	}


	{
		PERF_REGION_SCOPED_NAMED("Create ToneMap DescriptorSet");

		std::shared_ptr<ComputeShader> m_ToneMapp = m_Renderer->m_AcesShader.lock();

		struct ImageDescriptor litImageDescritptor
		{
			.texture = &m_ForwardTexture.color,
			.imageState = ImageState::General,
		};
	
		descriptorWrites.resize(1);
		descriptorWrites =
		{

			{
				ShaderProgramDescriptorType::StorageImage,
				0,
				litImageDescritptor,
			}
		};

		if (m_DescriptorSets.toneMap != nullptr)
			m_ToneMapp->FreeDescriptorSet(&m_DescriptorSets.toneMap);

		m_ToneMapp->AllocDescriptorSet(&m_DescriptorSets.toneMap, 0);
		m_DescriptorSets.toneMap->WriteDescriptorSets(descriptorWrites);
	}

	{
		PERF_REGION_SCOPED_NAMED("FinalViewPort DescriptorSet");
		std::shared_ptr<GraphicShader> drawToFinalImage = m_Renderer->m_DrawTextureScreenQuadShader.lock();

		const ImageSamplerDescriptor finalImageDescrotproSet
		{
			.sampler = sampler.get(),
			.texture = &m_ForwardTexture.color,
			.imageState = PC_CORE::ImageState::General
		};

		descriptorWrites.resize(1);
		descriptorWrites =
		{

			{
				ShaderProgramDescriptorType::CombinedImageSampler,
				0,
				finalImageDescrotproSet,
			}
		};

		if (m_DescriptorSets.finalViewPort != nullptr)
			drawToFinalImage->FreeDescriptorSet(&m_DescriptorSets.finalViewPort);

		drawToFinalImage->AllocDescriptorSet(&m_DescriptorSets.finalViewPort, 0);
		m_DescriptorSets.finalViewPort->WriteDescriptorSets(descriptorWrites);
	}

}


View::View(Renderer* _renderer, Tbx::Vector2i _viewPortSize) : m_Renderer(_renderer)
{
	Resize(_viewPortSize);
}
