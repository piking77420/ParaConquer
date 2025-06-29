#include "texture/vulkan_texture_3d.hpp"


const void* Vulkan::VulkanTexture3D::GetNativeHandle() const
{
    return &m_Texture;
}

Vulkan::VulkanTexture3D::VulkanTexture3D(const PC_CORE::CreateImageInfo3D& _createInfo)
{
    
}
