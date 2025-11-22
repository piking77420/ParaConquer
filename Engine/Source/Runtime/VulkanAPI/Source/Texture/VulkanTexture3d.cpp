#include "Texture/VulkanTexture3d.hpp"


Vulkan::VulkanTexture3D::VulkanTexture3D(const PC_CORE::CreateImageInfo& _createInfo) : RhiTexture3D(_createInfo),
    m_VulkanTexture(_createInfo)
{
    // TODO
    assert(_createInfo.TextureType == PC_CORE::TextureType::CubeMap
        || _createInfo.TextureType == PC_CORE::TextureType::CubeMapArray);
}
