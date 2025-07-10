#include "texture/vulkan_texture_2d.hpp"

#include "utils/helper_functions.hpp"
#include "utils/rhi_vulkan_parser.hpp"
#include "utils/transition_image_layout.hpp"
#include "utils/vulkan_buffer_helper.hpp"
#include "vulkan_context.hpp"
#include "utils/vulkan_image_helper.hpp"
#include "buffer/vulkan_buffer.hpp"
#include "low_renderer/rhi.hpp"

Vulkan::VulkanTexture2D::VulkanTexture2D(const PC_CORE::CreateImageInfo& _createTextureInfo) : m_VulkanTexture(_createTextureInfo)
{
	assert(_createTextureInfo.textureType == PC_CORE::TextureType::Texture2D
		|| _createTextureInfo.textureType == PC_CORE::TextureType::Array2D);
}
