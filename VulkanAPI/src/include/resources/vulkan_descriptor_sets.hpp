#pragma once

#include "vulkan_header.h"
#include "vulkan_shader_program.hpp"

namespace Vulkan
{
    struct VulkanDescriptorSets : public PC_CORE::ShaderProgramDescriptorSets
    {
        void WriteDescriptorSets(std::vector<PC_CORE::ShaderProgramDescriptorWrite> shaderProgramDescriptorSet) override;

        VulkanDescriptorSets() = default;
        
        ~VulkanDescriptorSets() override = default;

        std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptorSets;
    };

}

