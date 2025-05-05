#include "vulkan_frame_buffer.hpp"

#include "vulkan_context.hpp"
#include "vulkan_render_pass.hpp"
#include "low_renderer/rhi.hpp"
#include "handles/vulkan_image_handle.hpp"

Vulkan::VulkanFrameBuffer::VulkanFrameBuffer(const PC_CORE::CreateFrameInfo& _createFrameInfo)
	: FrameBuffer(_createFrameInfo)
{
	std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(VulkanContext::GetContext().rhiDevice);

	const VulkanRenderPass* renderPass = reinterpret_cast<const VulkanRenderPass*>(_createFrameInfo.renderPass);

	std::vector<PC_CORE::Texture*>* attachements = _createFrameInfo.attachements;



	int frame = 0;
	for (auto& framebuffer : m_FrameBuffers)
	{
		std::vector<vk::ImageView> image_views;
		image_views.reserve(attachements->size());

		for (auto& attachement : *attachements)
		{
			PC_CORE::GpuHandle* gpuHandle = attachement->GetHandle(frame).get();
			VulkanImageHandle* imageHandle = reinterpret_cast<VulkanImageHandle*>(gpuHandle);
			image_views.emplace_back(imageHandle->view);
				

		}

		vk::FramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = vk::StructureType::eFramebufferCreateInfo;
		framebufferCreateInfo.flags = {};
		framebufferCreateInfo.renderPass = renderPass->GetVulkanRenderPass();
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(image_views.size());
		framebufferCreateInfo.pAttachments = image_views.data();
		framebufferCreateInfo.width = _createFrameInfo.width;
		framebufferCreateInfo.height = _createFrameInfo.height;
		framebufferCreateInfo.layers = 1;
		framebuffer = vulkanDevice->GetDevice().createFramebuffer(framebufferCreateInfo);

		frame++;
	}
}


Vulkan::VulkanFrameBuffer::VulkanFrameBuffer(vk::Framebuffer _vkFramebuffer, uint32_t widht, uint32_t height, bool _ownBySwapChain) : m_FrameBuffers(_vkFramebuffer), m_OwnBySwapChain(_ownBySwapChain)
{
	m_Width = widht;
	m_Height = height;
}

Vulkan::VulkanFrameBuffer::~VulkanFrameBuffer()
{
	if (m_OwnBySwapChain)
		return;

	for (auto& framebuffer : m_FrameBuffers)
	{
		if (framebuffer == VK_NULL_HANDLE)
			continue;

		std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(VulkanContext::GetContext().rhiDevice);
		vulkanDevice->GetDevice().destroyFramebuffer(framebuffer);
		framebuffer = VK_NULL_HANDLE;
	}

}

vk::Framebuffer Vulkan::VulkanFrameBuffer::GetFramebuffer() const
{
	return m_FrameBuffers[PC_CORE::Rhi::GetFrameIndex()];
}
