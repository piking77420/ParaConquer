#include "rendering/renderer.hpp"

#include "io/window.hpp"
#include "rendering/render_harware_interface/descriptor_set.hpp"
#include "world/transform.hpp"
#include "rendering/render_harware_interface/vertex.hpp"
#include "resources/resource_manager.hpp"
#include "resources/shader_source.hpp"
#include "world/static_mesh.hpp"
#include "physics/box_collider.hpp"
#include "physics/sphere_collider.hpp"

#undef ERROR
#undef near;
#undef far;


using namespace PC_CORE;


void Renderer::Destroy()
{
	m_SwapChainCommandPool.~CommandPool();
	forwardRenderPass.~RenderPass();

	for (auto&& uniform : renderResources.sceneUniform)
		uniform.~UniformBuffer();

	RHI::DestroyInstance();
}

void Renderer::Render(const PC_CORE::RenderingContext& _renderingContext, const World& _world)
{
	m_CurrentImage = static_cast<size_t>(RHI::GetInstance().GetCurrentImage());
	UpdateUniforms(_renderingContext);


	BeginRenderPassInfo renderPassBeginInfo = {};
		renderPassBeginInfo.framebuffer = _renderingContext.frameBufferHandle;
	renderPassBeginInfo.renderArea.offset[0] = 0;
	renderPassBeginInfo.renderArea.offset[1] = 0;

	renderPassBeginInfo.renderArea.extend[0] = static_cast<int32_t>( _renderingContext.renderingContextSize.x);
	renderPassBeginInfo.renderArea.extend[1] = static_cast<int32_t>( _renderingContext.renderingContextSize.y);

	std::array<ClearValue, 2> clearColor;
	clearColor[0] = {0.5f, 0.5f, 0.5f, 1.0f};
	clearColor[1].clearDepthStencilValue = {1.0f, 0};
	renderPassBeginInfo.pClearValues = clearColor.data();
	renderPassBeginInfo.clearValueCount = clearColor.size();
	
	forwardRenderPass.Begin(*m_CommandBuffer, renderPassBeginInfo);

	const ViewPortExtend viewPortExtend =
	{
		.position = {},
		.width = static_cast<float>(_renderingContext.renderingContextSize.x),
		.height = static_cast<float>(_renderingContext.renderingContextSize.y),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	ScissorRect ScissorRect;
	ScissorRect.offset = {},
		ScissorRect.extend = {
			static_cast<uint32_t>(_renderingContext.renderingContextSize.x),
			static_cast<uint32_t>(_renderingContext.renderingContextSize.y)
	};

	 
	RHI::GetInstance().SetScissor(m_CommandBuffer->GetHandle(), ScissorRect);
	RHI::GetInstance().SetViewPort(m_CommandBuffer->GetHandle(), viewPortExtend);


	DrawStaticMesh(_renderingContext, _world);
	DrawWireFrame(_renderingContext, _world);
	
	forwardRenderPass.End(*m_CommandBuffer);
}

void Renderer::BeginFrame()
{
	static bool firstTime = false;
	if (glfwGetKey(Windowtpr->GetHandle(), GLFW_KEY_F5) == GLFW_PRESS && !firstTime)
	{
		RHI::GetInstance().WaitDevice();
		forwardShader->Reload(forwardRenderPass.GetHandle());
		firstTime = true;
	}

	if (glfwGetKey(Windowtpr->GetHandle(), GLFW_KEY_F5) == GLFW_RELEASE && firstTime)
	{
		firstTime = false;
	}

	RHI::GetInstance().WaitForAquireImage();
	m_CommandBuffer = &m_SwapChainCommandBuffers.at(static_cast<size_t>(RHI::GetInstance().GetCurrentImage()));
	RHI::GetInstance().BeginRender(m_CommandBuffer->GetHandle());
}

void Renderer::EndRender()
{
	RHI::GetInstance().EndRender();
}


void Renderer::SwapBuffers()
{
	std::vector<GPUBufferHandle> commandToWait;
	commandToWait.push_back(m_CommandBuffer->GetHandle());

	RHI::GetInstance().SwapBuffers(commandToWait.data(), static_cast<uint32_t>(commandToWait.size()));
}

void Renderer::WaitDevice()
{
	RHI::GetInstance().WaitDevice();
}

CommandBuffer& Renderer::GetCommandSwapChainBuffer()
{
	return m_SwapChainCommandBuffers.at(static_cast<size_t>(RHI::GetInstance().GetCurrentImage()));
}

GraphicAPI Renderer::GetGraphicsAPI()
{
	return m_GraphicApi;
}


void Renderer::InitRHiAndObject(GraphicAPI _graphicAPI, Window* _window)
{
	Windowtpr = _window;

	switch (_graphicAPI)
	{
	case PC_CORE::GraphicAPI::NONE:
		break;
	case PC_CORE::GraphicAPI::VULKAN:
	{
		Vulkan::VulkanAppCreateInfo createInfo =
		{
			.appName = "Editor",
			.engineName = "ParaConquer Engine",
			.windowPtr = _window->GetHandle(),
			.logCallback = &Renderer::RenderLog,
		};
		RHI::MakeInstance(new Vulkan::VulkanApp(createInfo));
	}
	break;
	case PC_CORE::GraphicAPI::COUNT:
		break;
	case GraphicAPI::DX3D12:
		break;
	default:
		break;
	}

	InitCommandPools();
}

void Renderer::InitShader()
{
	InitBasicShader();
	InitWireFrameShader();
}

void Renderer::InitBasicShader()
{
	ShaderSource* mainShaderVertex = ResourceManager::Get<ShaderSource>("main.vert");
	ShaderSource* mainShaderFrag = ResourceManager::Get<ShaderSource>("main.frag");

	PC_CORE::ProgramShaderCreateInfo createInfo{};
	createInfo.prograShaderName = "mainShader";

	createInfo.shaderInfo.shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS;
	ShaderGraphicPointInfo* shaderGraphicPointInfo = &std::get<ShaderGraphicPointInfo>(createInfo.shaderInfo.shaderInfoData);
	shaderGraphicPointInfo->polygonMode = PolygonMode::Fill;
	
	shaderGraphicPointInfo->vertexInputBindingDescritions.push_back(
		Vertex::GetBindingDescrition(0));
	shaderGraphicPointInfo->vertexAttributeDescriptions =
		Vertex::GetAttributeDescriptions(0);

	createInfo.renderPass = forwardRenderPass.GetHandle();
	createInfo.shaderInfo.descriptorInfo =
		{
		.freeDescriptorSet = true,
		.descriptorAllocCount = DESCRIPTOR_ALLOC_HIGH
		};
	

	forwardShader = new ShaderProgram(createInfo, { mainShaderVertex, mainShaderFrag });
	ResourceManager::Add<ShaderProgram>(forwardShader);
}

void Renderer::InitWireFrameShader()
{
	ShaderSource* wireframeShaderVertex = ResourceManager::Get<ShaderSource>("wireframe.vert");
	ShaderSource* wireframeShaderFrag = ResourceManager::Get<ShaderSource>("wireframe.frag");

	PC_CORE::ProgramShaderCreateInfo createInfo{};
	createInfo.prograShaderName = "wireframe";

	createInfo.shaderInfo.shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS;
	ShaderGraphicPointInfo* shaderGraphicPointInfo = &std::get<ShaderGraphicPointInfo>(createInfo.shaderInfo.shaderInfoData);
	shaderGraphicPointInfo->polygonMode = PolygonMode::Line;
	
	shaderGraphicPointInfo->vertexInputBindingDescritions.push_back(
		Vertex::GetBindingDescrition(0));
	shaderGraphicPointInfo->vertexAttributeDescriptions =
		Vertex::GetAttributeDescriptions(0);

	createInfo.renderPass = forwardRenderPass.GetHandle();

	wireframeShader = new ShaderProgram(createInfo, { wireframeShaderVertex, wireframeShaderFrag });
	ResourceManager::Add<ShaderProgram>(wireframeShader);
}

void Renderer::InitBuffer()
{
	for (auto&& uniform : renderResources.sceneUniform)
		uniform = UniformBuffer(sizeof(renderResources.sceneBufferGPU));
}

void Renderer::UpdateUniforms(const RenderingContext& _renderingContext)
{
	renderResources.sceneBufferGPU.view = LookAtRH(_renderingContext.lowLevelCamera.position,
		_renderingContext.lowLevelCamera.position + _renderingContext.lowLevelCamera.front,
		_renderingContext.lowLevelCamera.up);
	renderResources.sceneBufferGPU.proj = Tbx::PerspectiveMatrixFlipYAxis(_renderingContext.lowLevelCamera.fov,
		_renderingContext.lowLevelCamera.aspect,
		_renderingContext.lowLevelCamera.near,
		_renderingContext.lowLevelCamera.far);

	renderResources.sceneBufferGPU.deltatime = _renderingContext.deltaTime;
	renderResources.sceneBufferGPU.time = _renderingContext.time;
	renderResources.sceneUniform[m_CurrentImage].Update(sizeof(renderResources.sceneBufferGPU), 0, &renderResources.sceneBufferGPU);
}

void Renderer::DrawStaticMesh(const RenderingContext& _renderingContext, const PC_CORE::World& _world)
{
	forwardShader->Bind(m_CommandBuffer->GetHandle());
	forwardShader->BindDescriptorSet(m_CommandBuffer->GetHandle(), 0, 1,
		&descriptorSets[m_CurrentImage], 0, nullptr);

	const std::vector<StaticMesh>* staticMeshes = _world.scene.GetData<StaticMesh>();

	for (auto it = staticMeshes->begin(); it != staticMeshes->end(); it++)
	{
		if (it->mesh == nullptr)
			continue;

		const Entity* entity = _world.scene.GetEntityFromId(it->entityId);
		const Transform* transform = _world.scene.GetComponent<Transform>(entity);

		
		forwardShader->BindDescriptorSet(m_CommandBuffer->GetHandle(), 1, 1,
			&it->material->descriptorSetHandle[m_CurrentImage], 0, nullptr);
		
		Tbx::Matrix4x4f transformMatrix;
		Tbx::Trs3D(transform->position, transform->rotation.Normalize(), transform->scale, &transformMatrix);
		forwardShader->PushConstantMat4(m_CommandBuffer->GetHandle(), "modelMatrix", transformMatrix);

		m_CommandBuffer->BindVertexBuffer(it->mesh->vertexBuffer, 0, 1);
		m_CommandBuffer->BindIndexBuffer(it->mesh->indexBuffer);
		RHI::GetInstance().DrawIndexed(m_CommandBuffer->GetHandle(), it->mesh->indexBuffer.GetNbrOfIndicies(), 1, 0, 0, 0);
	}
}

void Renderer::DrawWireFrame(const RenderingContext& _renderingContext, const PC_CORE::World& _world)
{
	
	static Mesh* meshCube = ResourceManager::Get<Mesh>("cube.obj");
	static Mesh* meshSphere = ResourceManager::Get<Mesh>("sphere.obj");

	const std::vector<SphereCollider>* spheresColliders = _world.scene.GetData<SphereCollider>(); 
	const std::vector<BoxCollider>* boxsColliders = _world.scene.GetData<BoxCollider>();

	constexpr const char* wireframeDataPushConstant = "wireframeData";

	wireframeShader->Bind(m_CommandBuffer->GetHandle());
	m_CommandBuffer->BindVertexBuffer(meshSphere->vertexBuffer, 0, 1);
	m_CommandBuffer->BindIndexBuffer(meshSphere->indexBuffer);
	uint32_t nbrofIndices = meshSphere->GetNbrOfIndicies();
	

	wireFrameModelColor.color = {0,1,0,1};
	for (auto it = spheresColliders->begin(); it != spheresColliders->end(); it++)
	{ 
		const Entity* entity = _world.scene.GetEntityFromId(it->entityId);
		const Transform* transform = _world.scene.GetComponent<Transform>(entity);
		Tbx::Trs3D(transform->position, transform->rotation.Normalize(), transform->scale * it->radius, &wireFrameModelColor.model);
		wireframeShader->PushConstant(m_CommandBuffer->GetHandle(), wireframeDataPushConstant, wireFrameModelColor.color.GetPtr(), sizeof(decltype(wireFrameModelColor)));

		RHI::GetInstance().DrawIndexed(m_CommandBuffer->GetHandle(), nbrofIndices, 1, 0, 0, 0);
	}

	m_CommandBuffer->BindVertexBuffer(meshCube->vertexBuffer, 0, 1);
	m_CommandBuffer->BindIndexBuffer(meshCube->indexBuffer);
	nbrofIndices = meshCube->GetNbrOfIndicies();

	for (auto it = boxsColliders->begin(); it != boxsColliders->end(); it++)
	{
		const Entity* entity = _world.scene.GetEntityFromId(it->entityId);
		const Transform* transform = _world.scene.GetComponent<Transform>(entity);
		Tbx::Trs3D(transform->position + it->center, transform->rotation.Normalize(), transform->scale * it->extend, &wireFrameModelColor.model);
		wireframeShader->PushConstant(m_CommandBuffer->GetHandle(), wireframeDataPushConstant, wireFrameModelColor.color.GetPtr(), sizeof(decltype(wireFrameModelColor)));

		RHI::GetInstance().DrawIndexed(m_CommandBuffer->GetHandle(), nbrofIndices, 1, 0, 0, 0);
	}
	
}

void Renderer::CreateForwardPass()
{
	RenderPassCreateInfo createInfo;
	// Color and depth
	createInfo.attachmentDescriptions.resize(2);

	// SWAP
	createInfo.attachmentDescriptions[0].attachementUsage = AttachementUsage::COLOR;
	createInfo.attachmentDescriptions[0].format = RHIFormat::R8G8B8A8_SRGB;
	
	createInfo.attachmentDescriptions[1].attachementUsage = AttachementUsage::DEPTH;
	createInfo.attachmentDescriptions[1].format = RHIFormat::D32_SFLOAT_S8_UINT;
	
	// FORWARD PASS
	// COLOR PASS TO VIEWPORT
	createInfo.subPasses.resize(1);
	// FORWARD PASS
	createInfo.subPasses[0].shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS;
	createInfo.subPasses[0].attachementUsage.resize(2);
	createInfo.subPasses[0].attachementUsage.at(0) = AttachementUsage::COLOR;
	createInfo.subPasses[0].attachementUsage.at(1) = AttachementUsage::DEPTH;
	

	forwardRenderPass = RenderPass(createInfo);
}

void Renderer::InitRenderResources()
{

	CreateForwardPass();
	InitShader();
	InitBuffer();
	InitDescriptors();
}

void Renderer::InitCommandPools()
{
	const CommandPoolCreateInfo commandPoolCreateInfo =
	{
		.queueType = QueuType::GRAPHICS,
		.commandPoolBufferFlag = COMMAND_POOL_BUFFER_RESET,
	};
	m_SwapChainCommandPool = CommandPool(commandPoolCreateInfo);

	const CommandBufferCreateInfo commandBufferCreateInfo =
	{
		.commandBufferPtr = m_SwapChainCommandBuffers.data(),
		.commandBufferCount = static_cast<uint32_t>(m_SwapChainCommandBuffers.size()),
		.commandBufferlevel = CommandBufferlevel::PRIMARY
	};
	m_SwapChainCommandPool.AllocCommandBuffer(commandBufferCreateInfo);
}

void Renderer::InitDescriptors()
{
	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	forwardShader->CreateDescriptorSet(0, descriptorSets.data(), static_cast<uint32_t>(descriptorSets.size()));

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		constexpr size_t offset = 0;

		DescriptorBufferInfo descriptorBufferInfo = renderResources.sceneUniform[i].AsDescriptorBufferInfo(offset);
		DescriptorWriteSet descriptorWrite[1] =
		{

			{
				.dstDescriptorSetHandle = descriptorSets[i],
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,

				.descriptorBufferInfo = &descriptorBufferInfo,
				.descriptorImageInfo = nullptr,
				.descriptorTexelBufferViewInfo = nullptr
			},
		
		};

		PC_CORE::UpdateDescriptorSet(&descriptorWrite[0], 1);
	}
}


void Renderer::RenderLog(LogType _logType, const char* _message)
{
	switch (_logType)
	{
	case LogType::INFO:
		PC_LOG(_message)
			break;
	case LogType::ERROR:
		PC_LOGERROR(_message)
			break;
	default:;
	}
}