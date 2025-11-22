#include "Texture/VulkanTexture2d.hpp"

#include "Utils/HelperFunctions.hpp"
#include "Utils/RhiToVulkan.hpp"
#include "Utils/TransitionImageLayout.hpp"
#include "Utils/VulkanBufferHelper.hpp"
#include "VulkanContext.hpp"
#include "Utils/VulkanImageHelper.hpp"
#include "Buffer/VulkanBuffer.hpp"
#include "LowRenderer/Rhi.hpp"

Vulkan::VulkanTexture2D::VulkanTexture2D(const PC_CORE::CreateImageInfo& _createTextureInfo) :
    RhiTexture2D(_createTextureInfo),
    m_VulkanTexture(_createTextureInfo)
{
    assert(_createTextureInfo.TextureType == PC_CORE::TextureType::Texture2D);
}
