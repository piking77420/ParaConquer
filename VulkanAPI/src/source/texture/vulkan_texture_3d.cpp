#include "texture/vulkan_texture_3d.hpp"


const void* Vulkan::VulkanTexture3D::GetNativeHandle() const
{
    return &m_VulkanTexture;
}

Vulkan::VulkanTexture3D::VulkanTexture3D(const PC_CORE::CreateImageInfo& _createInfo) : m_VulkanTexture(_createInfo)
{
    // TODO
    assert(_createInfo.textureType == PC_CORE::TextureType::CubeMap
    || _createInfo.textureType == PC_CORE::TextureType::CubeMapArray);
    



}
