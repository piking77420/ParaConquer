#include "rendering/renderer.hpp"

#include "low_renderer/rhi.hpp"
#include "low_renderer/vertex.hpp"
#include "resources/resource_manager.hpp"
#include <thread>
#include "time/core_time.hpp"
#include "math/matrix_transformation.hpp"
#include "resources/texture.hpp"
#include "world/static_mesh.hpp"
#include "world/transform.hpp"

using namespace PC_CORE;




void Renderer::Init()
{
	m_RhiContext = Rhi::GetRhiContext();

	forwardPass = Rhi::CreateRenderPass(PC_CORE::RHIFormat::R8G8B8A8_UNORM, PC_CORE::RHIFormat::D32_SFLOAT);
	drawTextureScreenQuadPass = Rhi::CreateRenderPass(PC_CORE::RHIFormat::R8G8B8A8_UNORM);

	CreateForwardShader();
	CreateDrawQuadShader();

	constexpr CommandListCreateInfo commandListCreateInfo =
	{
		._commandPoolFamily = CommandPoolFamily::Graphics
	};

	primaryCommandList = PC_CORE::Rhi::CreateCommandList(commandListCreateInfo);
	m_ForwardShader->AllocDescriptorSet(&m_ShaderProgramDescriptorSet);

	m_UniformBuffer = UniformBuffer(&sceneBufferGPU, sizeof(sceneBufferGPU));

	UniformBufferDescriptor uniformBufferDescriptor
	{
		.buffer = &m_UniformBuffer,
	};

	Texture* diamondTexture = ResourceManager::Get<Texture>("diamond_block");

	if (diamondTexture == nullptr)
		return;

	ImageSamperDescriptor imageSamperDescriptor =
	{
	.sampler = Rhi::GetRhiContext()->sampler.get(),
	.texture = diamondTexture
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
			ShaderProgramDescriptorType::CombineImageSampler,
			1,
			nullptr,
			&imageSamperDescriptor
		}
	};

	m_ShaderProgramDescriptorSet->WriteDescriptorSets(descriptorSets);

}

void Renderer::Destroy()
{
	
}

bool Renderer::BeginDraw(Window* _window)
{
	bool hasObtainSwapChian = m_RhiContext->swapChain->GetSwapChainImageIndex(_window);
	if (hasObtainSwapChian)
	{
		primaryCommandList->Reset();
		primaryCommandList->BeginRecordCommands();
	}
	return hasObtainSwapChian;
}

void Renderer::DrawToRenderingContext(const PC_CORE::RenderingContext& renderingContext, Gbuffers* gbuffers, World* _world)
{
	sceneBufferGPU.time = PC_CORE::Time::GetTime();
	sceneBufferGPU.deltatime = PC_CORE::Time::DeltaTime();
	sceneBufferGPU.view = Tbx::LookAtRH<float>(renderingContext.lowLevelCamera.position,
		renderingContext.lowLevelCamera.position + renderingContext.lowLevelCamera.front, renderingContext.lowLevelCamera.up);
	sceneBufferGPU.proj = Tbx::PerspectiveMatrixFlipYAxis<float>(renderingContext.lowLevelCamera.fov, renderingContext.lowLevelCamera.aspect, renderingContext.lowLevelCamera.near, renderingContext.lowLevelCamera.far);

	m_UniformBuffer.Update(&sceneBufferGPU, sizeof(sceneBufferGPU));

	ClearValueFlags clearValueFlags = static_cast<ClearValueFlags>(ClearValueFlags::ClearValueColor | ClearValueFlags::ClearValueDepth);
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
	primaryCommandList->BindProgram(m_ForwardShader);

	const ViewportInfo viewportInfo =
	{
		.transform = {0, 0},
		.size = {static_cast<float>(renderingContext.renderingContextSize.x), static_cast<float>(renderingContext.renderingContextSize.y)},
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
		.scissorsOff = {0, 0},
		.scissorsextent = {renderingContext.renderingContextSize.x, renderingContext.renderingContextSize.y}
	};
	primaryCommandList->SetViewPort(viewportInfo);


	std::function<void(Transform&, StaticMesh&)> func = std::bind(&Renderer::DrawStaticMesh, this, std::placeholders::_1, std::placeholders::_2);
	_world->entityManager.ForEach<Transform, StaticMesh>(func);
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
	primaryCommandList->BindProgram(m_DrawTextureScreenQuadShader);
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
	primaryCommandList->BindDescriptorSet(m_DrawTextureScreenQuadShader, &_ShaderProgramDescriptorSets, 0, 1);
	primaryCommandList->Draw(4, 1, 0, 0);
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
	Tbx::Matrix4x4f modelMatrix;
	Tbx::Trs3D<float>(_transform.position, _transform.rotation,
		_transform.scale, &modelMatrix);
	primaryCommandList->PushConstant(m_ForwardShader, "PushConstants", &modelMatrix, sizeof(modelMatrix));
	
	
	primaryCommandList->BindVertexBuffer(_staticMesh.mesh->vertexBuffer, 0, 1);
	primaryCommandList->BindIndexBuffer(_staticMesh.mesh->indexBuffer, 0);
	primaryCommandList->BindDescriptorSet(m_ForwardShader, m_ShaderProgramDescriptorSet, 0, 1);
	primaryCommandList->DrawIndexed(_staticMesh.mesh->indexBuffer.GetIndexCount(), 1, 0, 0, 0);
}

