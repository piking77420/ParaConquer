#pragma once

#include "vulkan_header.h"
#include "vulkan_texture.hpp"
#include "low_renderer/rhi_texture_3d.hpp"

namespace Vulkan
{
    
    class VULKAN_API VulkanTexture3D : public PC_CORE::RhiTexture3D
    {
    public:
        const void* GetNativeHandle() const override;

        VulkanTexture3D(const PC_CORE::CreateImageInfo& _createInfo);

        VulkanTexture3D() = default;

        ~VulkanTexture3D() = default;
    
    private:
        VulkanTexture m_VulkanTexture;
    };
    
}
