#include "Rendering/View.hpp"

#include "Rendering/Renderer.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Time/CoreTime.hpp"
#include "Resources/ResourceManager.hpp"

using namespace PC_CORE;

void View::SetRenderingContextFlag(size_t _flag)
{
	renderingContext.renderingContextFlag = _flag;
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

	cameraGpu.time = renderingContext.time;
	cameraGpu.deltatime = renderingContext.deltaTime;

	const Tbx::Matrix4x4f view = Tbx::LookAtRH<float>(Tbx::Vector3f::Zero(),
		renderingContext.lowLevelCamera.front,
		renderingContext.lowLevelCamera.up);

	const Tbx::Matrix4x4f projection = Tbx::PerspectiveMatrixFlipYAxis<float>(renderingContext.lowLevelCamera.fov,
		renderingContext.lowLevelCamera.aspect,
		renderingContext.lowLevelCamera.near,
		renderingContext.lowLevelCamera.far);

	cameraGpu.vp = projection * view;
	cameraGpu.view = view;
	cameraGpu.proj = projection;
	cameraGpu.viewInv = cameraGpu.view.Invert();
	cameraGpu.projInv = cameraGpu.proj.Invert();
	cameraGpu.vpInv = cameraGpu.vp.Invert();
	cameraGpu.cameraNear = renderingContext.lowLevelCamera.near;
	cameraGpu.cameraFar = renderingContext.lowLevelCamera.far;
	cameraGpu.cameraPos = static_cast<Tbx::Vector3f>(renderingContext.lowLevelCamera.position);


	postProcessGpu.gamma = 2.2;
	postProcessGpu.exposure = 1;
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
	renderingContext.lowLevelCamera =
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
	renderingContext.time = PC_CORE::Time::GetTime();
	renderingContext.deltaTime = PC_CORE::Time::DeltaTime();

	// Frame Buffer
	renderingContext.forwardFrameBuffer = m_FrameBuffers.forwardFrameBuffer;
	renderingContext.gbufferFrameBuffer = m_FrameBuffers.gbufferFrameBuffer;
	renderingContext.finalImageFrameBuffer = m_FrameBuffers.finalImageFrameBuffer;

	// Descriptor
	renderingContext.geometryDescritproSet = m_DescriptorSets.geometryPass;
	renderingContext.defferdLightingGbufferSet = m_DescriptorSets.defferedPassGbuffers;
	renderingContext.defferdLightingLightingCameraSet = m_DescriptorSets.defferedPassCameraLight;
	renderingContext.forwardDesritptorSet = m_DescriptorSets.forwardDescriptor;
	renderingContext.toneMapDescritptorSet = m_DescriptorSets.toneMap;
	renderingContext.finalImageDescritptorSet = m_DescriptorSets.finalViewPort;

	renderingContext.hdrImage = &forwardTexture.color;
	renderingContext.renderingContextSize = { static_cast<uint32_t>(m_CurrentSize.x), static_cast<uint32_t>(m_CurrentSize.y) };
}

void View::CreateImages()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Rendering);

	gbuffers.CreateGBuffers(m_CurrentSize);

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

		forwardTexture.color = Texture2D(createInfo);

		createInfo.format = RHIFormat::D32_SFLOAT;
		createInfo.channel = Channel::GREY;
		createInfo.textureUsage = PC_CORE::TextureUsage::Depth;

		forwardTexture.depth = Texture2D(createInfo);
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

		  finalImage = PC_CORE::Texture2D(createInfo);
		  createInfo.samples = PC_CORE::Rhi::GetRhiContext()->physicalDevices->GetPhysicalDevice().GetMaxUsableSampleCount();
		  createInfo.textureUsage = PC_CORE::TextureUsage::RenderTarget | PC_CORE::TextureUsage::Sampled;
		  resolvedImages = PC_CORE::Texture2D(createInfo);

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
			attechementDescriptor[i].texture = &gbuffers.gbuffers[i];


		attechementDescriptor[i++].texture = &forwardTexture.color;
		attechementDescriptor[i++].texture = &forwardTexture.depth;

		assert(&forwardTexture.depth == attechementDescriptor[attechementDescriptor.size() - 1].texture &&
			"Backend expect thaht depth is the last attachement");

		createFrameBufferInfo.attachements = &attechementDescriptor;
		createFrameBufferInfo.renderPass = m_Renderer->renderPasses.defferedPass.get();


		m_FrameBuffers.gbufferFrameBuffer = Rhi::CreateFrameBuffer(createFrameBufferInfo);
	}

	// Forward FrameBuffer
	{
		std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;
		attechementDescriptor.resize(2); // color + depth;
		attechementDescriptor[0].texture = &forwardTexture.color;
		attechementDescriptor[1].texture = &forwardTexture.depth;


		createFrameBufferInfo.attachements = &attechementDescriptor;
		createFrameBufferInfo.renderPass = m_Renderer->renderPasses.forwardPass.get();

		m_FrameBuffers.forwardFrameBuffer = Rhi::CreateFrameBuffer(createFrameBufferInfo);
	}

	

	// Final Image
	{
		std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;
		attechementDescriptor.resize(2); // color + depth;

		attechementDescriptor[0].texture = &resolvedImages;
		attechementDescriptor[1].texture = &finalImage;

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
			.image = &gbuffers.gbuffers[i],
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
				0,
				lightData,
			}
		};

		if (m_DescriptorSets.defferedPassCameraLight != nullptr)
			deferredShader->FreeDescriptorSet(&m_DescriptorSets.defferedPassCameraLight);

		deferredShader->AllocDescriptorSet(&m_DescriptorSets.defferedPassCameraLight, SCENE_DESCRIPTOR_SET);
		m_DescriptorSets.defferedPassCameraLight->WriteDescriptorSets(descriptorWrites);
	}

	{
		/*
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
		m_DescriptorSets.forwardDescriptor->WriteDescriptorSets(descriptorWrites);*/
	}

	
	{
		PERF_REGION_SCOPED_NAMED("Create ToneMap DescriptorSet");

		std::shared_ptr<ComputeShader> m_ToneMapp = m_Renderer->m_AcesShader.lock();

		struct ImageDescriptor hdrImage
		{
			.texture = &forwardTexture.color,
			.imageState = ImageState::General,
		};

	
		descriptorWrites.resize(1);
		descriptorWrites =
		{

			{
				ShaderProgramDescriptorType::StorageImage,
				0,
				hdrImage,
			},
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
			.texture = &forwardTexture.color,
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
