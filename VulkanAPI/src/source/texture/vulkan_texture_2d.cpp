#include "texture/vulkan_texture_2d.hpp"

#include "utils/helper_functions.hpp"
#include "utils/rhi_vulkan_parser.hpp"
#include "utils/transition_image_layout.hpp"
#include "utils/vulkan_buffer_helper.hpp"
#include "vulkan_context.hpp"
#include "utils/vulkan_image_helper.hpp"
#include "buffer/vulkan_buffer.hpp"
#include "low_renderer/rhi.hpp"

Vulkan::VulkanTexture2D::VulkanTexture2D(const PC_CORE::CreateImageInfo2D& _createTextureInfo) : m_VulkanTexture(_createTextureInfo.memoryVisibility)
{
	if (_createTextureInfo.depth < 1)
	{
		PC_LOGERROR("Vulkan::VulkanGpuAllocator::CreateTexture: _createTextureInfo.depth < 1 )");
	}

	auto& context = VulkanContext::GetContext();
	vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->GetDevice();

	// parse values
	const vk::Format format = Utils::RHIFormatToVkFormat(_createTextureInfo.format);
	const uint32_t mipLevel = _createTextureInfo.GenerateMipMap ? _createTextureInfo.mipsLevels : 1;
	const vk::SampleCountFlagBits sampleCount = Utils::RhiSampleCountToVuklan(_createTextureInfo.samples);
	vk::ImageUsageFlags textureUsage = GetMemoryPropertyFlags(_createTextureInfo.textureUsage);

	// if generate mip maps then image need to be transfer src
	textureUsage = mipLevel > 1 ? textureUsage | vk::ImageUsageFlagBits::eTransferSrc : textureUsage;

	const VmaMemoryUsage vmaMemoryUsage = GetTextureMemoryUsage(_createTextureInfo.memoryVisibility);
	const vk::ImageAspectFlags aspectFlags = GetImageAspectFlags(_createTextureInfo.textureUsage);

	vk::ImageLayout finalImageLayout = GetImageLayout(_createTextureInfo.textureUsage);

	for (size_t i = 0; i < m_VulkanTexture.textureAndAlloc.size(); i++)
	{
		CreateImage(context.allocator, _createTextureInfo.width
			, _createTextureInfo.height, _createTextureInfo.depth, _createTextureInfo.mipsLevels,
			sampleCount,
			vk::ImageType::e2D,
			format, vk::ImageTiling::eOptimal,
			textureUsage, vmaMemoryUsage,
			reinterpret_cast<VkImage*>(&m_VulkanTexture.textureAndAlloc[i].image), &m_VulkanTexture.textureAndAlloc[i].allocation);
	}

	const Utils::SingleCommandBeginInfo singleCommandBeginInfo =
	{
			.device = device,
			.commandPool = context.transferCommandPool,
			.queue = context.mainQueue
	};


	if (_createTextureInfo.data != nullptr)
	{
		vk::ImageLayout beginImageLayout = vk::ImageLayout::eUndefined;

#pragma region SendData
		const int multiplayer = GetMultiplayer(_createTextureInfo.channel);
		const size_t imageSize = static_cast<size_t>(_createTextureInfo.width * _createTextureInfo.height * _createTextureInfo.depth * multiplayer);


		std::vector<BufferAndAlloc> bufferAndAllocs;
		bufferAndAllocs.resize(m_VulkanTexture.textureAndAlloc.size());

		std::vector<void*> mappedData;
		mappedData.resize(m_VulkanTexture.textureAndAlloc.size());


		for (size_t i = 0; i < m_VulkanTexture.textureAndAlloc.size(); i++)
		{
			VkBuffer* buffPtr = reinterpret_cast<VkBuffer*>(&bufferAndAllocs[i].buffer);
			Utils::CreateBuffer(context.allocator, imageSize, vk::BufferUsageFlagBits::eTransferSrc, VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU,
				buffPtr, &bufferAndAllocs[i].alloc);

			vmaMapMemory(context.allocator, bufferAndAllocs[i].alloc, &mappedData[i]);
			memcpy(mappedData[i], _createTextureInfo.data, imageSize);
			vmaUnmapMemory(context.allocator, bufferAndAllocs[i].alloc);
		}

		vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);

		for (size_t i = 0; i < m_VulkanTexture.textureAndAlloc.size(); i++)
		{
			const uint32_t w = _createTextureInfo.width;
			const uint32_t h = _createTextureInfo.height;
			const uint32_t d = _createTextureInfo.depth;

			vk::BufferImageCopy region{};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = aspectFlags;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = VkOffset3D{ 0, 0, 0 };
			region.imageExtent = vk::Extent3D{
				w,
				h,
				d
			};
			TransitionImageLayout(commandBuffer, m_VulkanTexture.textureAndAlloc[i].image, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, aspectFlags, mipLevel);
			commandBuffer.copyBufferToImage(bufferAndAllocs[i].buffer, m_VulkanTexture.textureAndAlloc[i].image, vk::ImageLayout::eTransferDstOptimal, region);
			if (mipLevel != 1)
			{
				//transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
				GenerateMipMap(commandBuffer, m_VulkanTexture.textureAndAlloc[i].image, _createTextureInfo.width, _createTextureInfo.height, format, mipLevel, aspectFlags, finalImageLayout);
			}
			else
			{
				TransitionImageLayout(commandBuffer, m_VulkanTexture.textureAndAlloc[i].image, format, beginImageLayout, finalImageLayout, aspectFlags, mipLevel);
			}
		}
		EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);

		// Single command ended usless buffer now
		for (auto& it : bufferAndAllocs)
		{
			vmaDestroyBuffer(context.allocator, it.buffer, it.alloc);
		}

#pragma endregion SendData 
	}
	else
	{
		vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);
		for (size_t i = 0; i < m_VulkanTexture.textureAndAlloc.size(); i++)
		{
			TransitionImageLayout(commandBuffer, m_VulkanTexture.textureAndAlloc[i].image, format, vk::ImageLayout::eUndefined, finalImageLayout, aspectFlags, mipLevel);
		}
		EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);
	}

	for (auto& textureAndAlloc : m_VulkanTexture.textureAndAlloc)
		textureAndAlloc.imageView = CreateImageView(device, textureAndAlloc.image, vk::ImageViewType::e2D, format, aspectFlags, mipLevel);

}
