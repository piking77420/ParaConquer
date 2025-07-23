#pragma once

#include "vulkan_texture.hpp"
#include "low_renderer/rhi_texure_2d.hpp"

namespace Vulkan
{
    class VulkanTexture2D : public PC_CORE::RhiTexture2D
    {
    public:

        DEFAULT_COPY_MOVE_OPERATIONS(VulkanTexture2D)

        PC_CORE_API const void* GetNativeHandle() const override
        {
            return &m_VulkanTexture;
        }
        
        PC_CORE_API void* GetNativeHandle() override
        {
            return &m_VulkanTexture;
        }

        PC_CORE_API const VulkanTexture& GetHandle() const
        {
            return m_VulkanTexture;
        }
        
        VulkanTexture2D(const PC_CORE::CreateImageInfo& _createTextureInfo);
    
        VulkanTexture2D() = default;

        ~VulkanTexture2D() override = default;


    private:
        VulkanTexture m_VulkanTexture;
    };

}

