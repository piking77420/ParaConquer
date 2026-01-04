#pragma once

#include <Map>
#include <Stack>
#include <spirv_reflect.h>

#include "VulkanHeader.h"
#include "LowRenderer/RhiDescriptorSet.hpp"



namespace Vulkan
{
    class VulkanContext;

    class VulkanDescritptorManager
    {
    public:
        explicit VulkanDescritptorManager(VulkanContext& _Context);

        ~VulkanDescritptorManager();

        void CreateDesciptorPool();

        std::vector<vk::DescriptorSetLayout> GetDescriptorLayouts(const std::vector<SpvReflectShaderModule>& _SpvReflectShaderModule);

        vk::DescriptorSetLayout GetDescriptorLayout(const std::vector<PC_CORE::DescriptorWrite>& _DescriptorWrites);

        vk::DescriptorPool GetVkDesciptorPool() const
        {
            return m_Pool;
        }

    private:
        VulkanContext& m_Context;

        std::unordered_map<uint32_t, vk::DescriptorSetLayout> m_Layouts;

        vk::DescriptorPool m_Pool;

        size_t HashLayout(const std::vector<vk::DescriptorSetLayoutBinding>& _DescriptorWrites);

        vk::DescriptorSetLayout CreateLayout(const std::vector<vk::DescriptorSetLayoutBinding>& _Bindings);

    };
}
