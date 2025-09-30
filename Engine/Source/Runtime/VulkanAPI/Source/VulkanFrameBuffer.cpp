#include "VulkanFrameBuffer.hpp"

#include "VulkanContext.hpp"
#include "VulkanRenderPass.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Texture/VulkanTexture2d.hpp"

Vulkan::VulkanFrameBuffer::VulkanFrameBuffer(const PC_CORE::CreateFrameInfo& _createFrameInfo)
	: FrameBuffer(_createFrameInfo)
{
	std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(VulkanContext::GetContext().rhiDevice);

	const VulkanRenderPass* renderPass = reinterpret_cast<const VulkanRenderPass*>(_createFrameInfo.renderPass);
	
	int frame = 0;
	for (auto& framebuffer : m_FrameBuffers)
	{
		std::vector<vk::ImageView> image_views;
		image_views.reserve(_createFrameInfo.attachements->size());
		

		
		for (auto& attachement : *_createFrameInfo.attachements)
		{
			const VulkanTexture& texture = *reinterpret_cast<const VulkanTexture*>(attachement.texture->GetRhiHandle()->GetNativeHandle());
			assert(texture.textureAndAlloc[frame].imageView != VK_NULL_HANDLE);

			image_views.emplace_back(texture.textureAndAlloc[frame].imageView);
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


Vulkan::VulkanFrameBuffer::VulkanFrameBuffer(vk::Framebuffer _vkFramebuffer, uint32_t widht, uint32_t height) : m_FrameBuffers(_vkFramebuffer)
{
	m_Width = widht;
	m_Height = height;
}

Vulkan::VulkanFrameBuffer::~VulkanFrameBuffer()
{
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
