#pragma once

#include "vulkan_header.h"
#include "vulkan_shader_program.hpp"

namespace Vulkan
{
    struct VulkanDescriptorSets : public PC_CORE::ShaderProgramDescriptorSets
    {

        VULKAN_API const void* GetNativeHandle() const override;

        VULKAN_API void* GetNativeHandle() override;


        VULKAN_API void WriteDescriptorSets(const std::vector<PC_CORE::ShaderProgramDescriptorWrite>& shaderProgramDescriptorSet) override;

        VULKAN_API VulkanDescriptorSets() = default;
        
        VULKAN_API ~VulkanDescriptorSets() override = default;

    private:
        std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> m_DescriptorSets;

    };

}

