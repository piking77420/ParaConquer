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
#include "VulkanDescriptorSet.hpp"
#include "VulkanSampler.hpp"
#include "VulkanShaderProgram.hpp"
#include "Utils/HelperFunctions.hpp"

using namespace PC_CORE;


Rhi::~Rhi()
{

}

Rhi::Rhi(const RenderHardwareInterfaceCreateInfo& _CreateInfo)
	: m_GraphicsApi(_CreateInfo.GraphicsAPI)
{
	PERF_REGION_SCOPED;
	PC_LOG("Rhi Initialize");

	RenderInstanceCreateInfo renderInstanceCreateInfo =
	{
		.appName = _CreateInfo.appName,
		.gpuDebug = _CreateInfo.gpuDebug
	};

	const PhysicalDevicesCreateInfo physicalDevicesCreateInfo =
	{
		{
			// RhiExtension::RayTracing,
			RhiExtension::MeshShader
		},

	};

	const RhiContextCreateInfo renderContextCreateInfo =
	{
		_CreateInfo.window->GetHandle(),
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


RhiShaderProgram* Rhi::CreateRhiShaderProgram()
{

	switch (m_GraphicsApi)
	{
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanShaderProgram(*this);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::None:
	case GraphicAPI::Count:
		break;
	}

	return nullptr;
}

RhiDescriptorSet* Rhi::CreateDescriptorSet()
{
	switch (m_GraphicsApi)
	{
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanDescriptorSet(*this);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::None:
	case GraphicAPI::Count:
		break;
	}

	return nullptr;
}

CommandList* Rhi::CreateCommandList()
{

	switch (m_GraphicsApi)
	{
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanCommandList(*this);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::None:
	case GraphicAPI::Count:
		throw std::runtime_error("Invalid GraphicAPI");
	}

	return nullptr;
}

RhiRenderPass* Rhi::CreateRenderPass(RhiFormat _colorFormat, RhiFormat _depthFormat)
{

	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanRenderPass(*this, _colorFormat, _depthFormat);
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

RhiRenderPass* Rhi::CreateRenderPass(RhiFormat _colorFormat)
{

	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanRenderPass(*this, _colorFormat);
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

PC_CORE_API RhiRenderPass* Rhi::CreateRenderPass(RhiFormat _colorFormat, uint32_t sampleCount)
{
	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanRenderPass(*this, _colorFormat, sampleCount);
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

RhiRenderPass* Rhi::CreateRenderPass()
{
	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanRenderPass(*this);
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

RhiFrameBuffer* Rhi::CreateFrameBuffer()
{
	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanFrameBuffer(*this);
		break;
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default: assert(false);
	}

	return nullptr;
}


RhiBuffer* Rhi::CreateBuffer()
{
	static_assert(std::is_base_of_v<RhiBuffer, Vulkan::VulkanBuffer>, "");

	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanBuffer(*this);
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default:
		return nullptr;
	}

	return nullptr;
}

RhiTexture* Rhi::CreateTexture()
{
	static_assert(std::is_base_of_v<RhiTexture, Vulkan::VulkanTexture>, "");

	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanTexture(*this);
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default:
		return nullptr;
	}

	return nullptr;
}

RhiSampler* Rhi::CreateSampler()
{
	static_assert(std::is_base_of_v<RhiSampler, Vulkan::VulkanSampler>, "");

	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanSampler(*this);
	case GraphicAPI::D3d12:
		break;
	case GraphicAPI::Count:
		break;
	default:;
	}

	return nullptr;
}

PC_CORE_API RhiFence* Rhi::CreateFence()
{
	switch (m_GraphicsApi)
	{
	case GraphicAPI::None:
		break;
	case GraphicAPI::Vulkan:
		return new Vulkan::VulkanFence(*this);
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

bool Rhi::IsYUpNdc() const
{
	switch (m_GraphicsApi)
	{
	case PC_CORE::GraphicAPI::Vulkan:
		return false;
	}

	return true;
}

bool Rhi::IsYUpFrameBuffer() const
{
	return false;
}

uint32_t Rhi::ComputeTextureLevel(uint32_t Width, uint32_t Height) const
{
	return static_cast<uint32_t>(std::floor(std::log2(std::max(Width, Height)))) + 1;
}

void Rhi::VulkanInitialize(const RhiContextCreateInfo& _CreateInfo)
{
	m_RhiContext.reset(new Vulkan::VulkanContext(*this));
	Vulkan::VulkanContext* VkContext = reinterpret_cast<Vulkan::VulkanContext*>(m_RhiContext.get());
	VkContext->Init(_CreateInfo);
}

void Rhi::DX12Initialize(const RhiContextCreateInfo& _CreateInfo)
{
}
