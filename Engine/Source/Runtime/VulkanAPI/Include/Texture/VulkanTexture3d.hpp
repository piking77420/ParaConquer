#pragma once

#include "VulkanHeader.h"
#include "VulkanTexture.hpp"
#include "LowRenderer/RhiTexture3d.hpp"

namespace Vulkan
{
    class VULKAN_API VulkanTexture3D : public PC_CORE::RhiTexture3D
    {
    public:
        const void* GetNativeHandle() const override
        {
            return &m_VulkanTexture;
        }

        void* GetNativeHandle() override
        {
            return &m_VulkanTexture;
        }

        VulkanTexture3D(const PC_CORE::CreateImageInfo& _createInfo);

        VulkanTexture3D() = default;

        ~VulkanTexture3D() override = default;

    private:
        VulkanTexture m_VulkanTexture;
    };
}
