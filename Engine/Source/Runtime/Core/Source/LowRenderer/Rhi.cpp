#include "LowRenderer/Rhi.hpp"

#include "Log.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanCommandList.hpp"
#include "VulkanContext.hpp"
#include "VulkanFrameBuffer.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanFence.hpp"
#include "VulkanTexture.hpp"
#include "VulkanSwapChain.hpp"
#include "Resources/VulkanDescriptorSets.hpp"
#include "Resources/VulkanSampler.hpp"
#include "Resources/VulkanShaderProgram.hpp"
#include "Utils/HelperFunctions.hpp"

using namespace PC_CORE;


Rhi::Rhi(const RenderHardwareInterfaceCreateInfo& _createInfo) : m_GraphicsApi(_createInfo.GraphicsAPI)
{
	PERF_REGION_SCOPED;
	PC_LOG("Rhi Initialize")
	Init(_createInfo);
}

Rhi::~Rhi()
{

}

RhiSwapChain* Rhi::CreateRhiSwapChain()
{
	switch (m_GraphicsApi)
	{
		break;
	case GraphicAPI::Vulkan:
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::None:
	case GraphicAPI::Count:
		break;
	}

	return nullptr;
}


RhiShaderProgram* Rhi::CreateRhiShaderProgram(const std::string& _programName, const RhiShaderProgram::ProgramShaderCreateInfo& _programShaderCreateInfo)
{

	switch (m_GraphicsApi)
	{
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanShaderProgram(*this, _programName, _programShaderCreateInfo);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::None:
	case GraphicAPI::Count:
		break;
	}

	return nullptr;
}

RhiShaderProgram* Rhi::CreateRhiShaderProgram(std::string&& _programName, const RhiShaderProgram::ProgramShaderCreateInfo& _programShaderCreateInfo)
{
	switch (m_GraphicsApi)
	{
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanShaderProgram(*this, std::move(_programName), _programShaderCreateInfo);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::None:
	case GraphicAPI::Count:
		break;
	}

	return nullptr;
}

CommandList* Rhi::CreateCommandList(const std::string& _name, const CommandListCreateInfo& _commandListCreateInfo)
{

	switch (m_GraphicsApi)
	{
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanCommandList(*this, _name, _commandListCreateInfo);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::None:
	case GraphicAPI::Count:
		throw std::runtime_error("Invalid GraphicAPI");
	}

	return nullptr;
}

CommandList* Rhi::CreateCommandList(std::string&& _name, const CommandListCreateInfo& _commandListCreateInfo)
{

	switch (m_GraphicsApi)
	{
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanCommandList(*this, std::move(_name), _commandListCreateInfo);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::None:
	case GraphicAPI::Count:
		throw std::runtime_error("Invalid GraphicAPI");
	}

	return nullptr;
}

RhiRenderPass* Rhi::CreateRenderPass(const std::string& _name, RhiFormat _colorFormat, RhiFormat _depthFormat)
{

	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanRenderPass(*this, _name, _colorFormat, _depthFormat);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default:
		assert(false);
	}

	return nullptr;
}

RhiRenderPass* Rhi::CreateRenderPass(const std::string& _name, RhiFormat _colorFormat)
{


	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanRenderPass(*this, _name, _colorFormat);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default:
		assert(false);
	}
	return nullptr;
}

PC_CORE_API RhiRenderPass* Rhi::CreateRenderPass(const std::string& _name, RhiFormat _colorFormat, uint32_t sampleCount)
{


	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanRenderPass(*this, _name, _colorFormat, sampleCount);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default:
		assert(false);
	}
	return nullptr;
}

RhiRenderPass* Rhi::CreateRenderPass(const std::string& _name, const RenderPassDescriptor& _renderPassDescriptor)
{


	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanRenderPass(*this, _name, _renderPassDescriptor);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default:
		assert(false);
	}

	return nullptr;
}

RhiFrameBuffer* Rhi::CreateFrameBuffer(const std::string& _name, const CreateFrameInfo& _createFrameInfo)
{
	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanFrameBuffer(*this, _name, _createFrameInfo);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default: assert(false);
	}

	return nullptr;
}

RhiBuffer* Rhi::CreateBuffer(const std::string& _name, const RhiBuffer::RhiBufferDescriptor& _rhiBufferDescriptor,
	RhiResource::MemoryUsage _memoryUsage)
{

	static_assert(std::is_base_of_v<RhiBuffer, Vulkan::VulkanBuffer>, "");

	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanBuffer(*this, _name, _rhiBufferDescriptor, _memoryUsage);
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default:
		return nullptr;
	}

	return nullptr;
}

RhiTexture* Rhi::CreateTexture(const std::string& _name, const RhiTexture::RhiTextureDesciptor& _rhiTextureDesciptor, RhiResource::MemoryUsage _memoryUsage)
{

	static_assert(std::is_base_of_v<RhiTexture, Vulkan::VulkanTexture>, "");

	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanTexture(*this, _name, _rhiTextureDesciptor, _memoryUsage);
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default:
		return nullptr;
	}

	return nullptr;
}

PC_CORE_API RhiTexture* Rhi::CreateTexture(std::string&& _name, const RhiTexture::RhiTextureDesciptor& _rhiTextureDesciptor, RhiResource::MemoryUsage _memoryUsage)
{

	static_assert(std::is_base_of_v<RhiTexture, Vulkan::VulkanTexture>, "");

	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanTexture(*this, std::move(_name), _rhiTextureDesciptor, _memoryUsage);
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default:
		return nullptr;
	}

	return nullptr;
}

RhiSampler* Rhi::CreateSampler(const std::string& _name, const SamplerCreateInfo& _samplerCreateInfo)
{
	static_assert(std::is_base_of_v<RhiSampler, Vulkan::VulkanSampler>, "");

	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanSampler(*this, _name, _samplerCreateInfo);
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default:;
	}

	return nullptr;
}

PC_CORE_API RhiFence* Rhi::CreateFence(const std::string& _name, const RhiFenceCreateInfo& _rhiFenceCreateInfo)
{
	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanFence(*this, _name, _rhiFenceCreateInfo);
	case GraphicAPI::D3d12:

	case GraphicAPI::Count:
		assert(false);
		break;
	default:;
	}

	return nullptr;
}


RhiContext& Rhi::GetRhiContext()
{
	return *m_RhiContext.get();
}

const RhiContext& Rhi::GetRhiContext() const
{
	return *m_RhiContext.get();
}

void Rhi::NextFrame()
{
	m_CurrentFrame = (m_CurrentFrame + 1) % MaxFramesInFlight;
}

void Rhi::PushResourceUpdate(const std::function<void(CommandList*)>& _resourceUpdates)
{
	m_ResourceUpdateStack.push_back(_resourceUpdates);
}

void Rhi::ProcessResourceUpdate()
{
	m_ResourceUpdateCommandList->BeginRecordCommands();
	for (auto& RUpdate : m_ResourceUpdateStack)
	{
		RUpdate(m_ResourceUpdateCommandList.get());
	}
	m_ResourceUpdateCommandList->EndRecordCommands();
	m_ResourceUpdateStack.clear();

	m_ResourceUpdateCommandList->Flush(*m_ResourceUpdateFence.get());
}

void Rhi::Init(const RenderHardwareInterfaceCreateInfo& _createInfo)
{
	RenderInstanceCreateInfo renderInstanceCreateInfo =
	{
		.appName = _createInfo.appName,
		.gpuDebug = _createInfo.gpuDebug
	};

	const PhysicalDevicesCreateInfo physicalDevicesCreateInfo =
	{
		{
			// RhiExtension::RayTracing,
			//RhiExtension::MeshShader
		},

	};

	const RhiContextCreateInfo renderContextCreateInfo =
	{
		_createInfo.window->GetHandle(),
		&renderInstanceCreateInfo,
		&physicalDevicesCreateInfo
	};


	switch (m_GraphicsApi)
	{
	case GraphicAPI::Vulkan:
		VulkanInitialize(renderContextCreateInfo);
		break;
	case GraphicAPI::D3d12:
		DX12Initialize(renderContextCreateInfo);
		break;
	case GraphicAPI::Count:
	case GraphicAPI::None:
		break;
	}

	CommandListCreateInfo commandListCreateInfo;
	commandListCreateInfo.CommandPoolFamily = CommandPoolFamily::Graphics;
	commandListCreateInfo.CommandBufferType = CommandBufferType::Primary;

	m_ResourceUpdateCommandList.reset(Rhi::CreateCommandList("Resource Operation", commandListCreateInfo));
	m_ResourceUpdateCommandList->Build();

	RhiFenceCreateInfo fenceInfo;
	fenceInfo.signaled = true;

	m_ResourceUpdateFence.reset(Rhi::CreateFence("ResourceUpdateFence", fenceInfo));
	m_ResourceUpdateFence->Build();
}

void Rhi::VulkanInitialize(const RhiContextCreateInfo& _createInfo)
{
	m_RhiContext.reset(new Vulkan::VulkanContext(*this, _createInfo));
}

void Rhi::DX12Initialize(const RhiContextCreateInfo& _createInfo)
{
}
