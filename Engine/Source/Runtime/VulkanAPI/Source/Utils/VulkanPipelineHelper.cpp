#include <memory>
#include <Utils/VulkanPipelineHelper.hpp>
#include <Utils/RhiToVulkan.hpp>
#include <LowRenderer/Rhi.hpp>
#include <Resources/ShaderSourceBinary.hpp>
#include <VulkanContext.hpp> 
#include <VulkanDevice.hpp>
#include <VulkanDescritptorManager.hpp>

namespace Vulkan::Utils
{
    void ParsePushConstantRange(VulkanShaderProgramCreateContext& _vulkanShaderProgramCreateContex)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Rhi);

        std::unordered_map<std::string_view, vk::PushConstantRange> map;

        std::vector<vk::PushConstantRange>* pushConstantRanges = &_vulkanShaderProgramCreateContex.pushConstantRanges;

        uint32_t pushConstantRangeCount = 0;
        for (size_t i = 0; i < _vulkanShaderProgramCreateContex.modulesReflected.size(); i++)
        {
            const auto& Module = _vulkanShaderProgramCreateContex.modulesReflected[i];
            for (uint32_t j = 0; j < Module.push_constant_block_count; j++)
            {
                const SpvReflectBlockVariable& block = Module.push_constant_blocks[j];
                const std::string_view BlockName = block.name;
                auto it = map.find(BlockName);
                if (it != map.end())
                {
                    assert(it->second.size == block.size);
                    assert(it->second.offset == block.offset);
                    it->second.stageFlags |= static_cast<vk::ShaderStageFlags>(Module.shader_stage);
                }
                else
                {
                    auto& pushConstant = map[BlockName];
                    pushConstant.offset = block.offset;
                    pushConstant.size = block.size;
                    pushConstant.stageFlags = static_cast<vk::ShaderStageFlags>(Module.shader_stage);
                }
            }
        }
        pushConstantRanges->resize(map.size());
        size_t Index = 0;
        for (auto& E : map)
        {
            (*pushConstantRanges)[Index++] = E.second;
        }

    }

	VulkanShaderProgramCreateContext CreateContext(vk::Device _Device, const std::vector<const PC_CORE::ShaderSourceBinary*>& _programShaderCreateInfo)
	{
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Rhi);

        const size_t shaderStageCount = _programShaderCreateInfo.size();

        VulkanShaderProgramCreateContext vulkanShaderProgramCreateContex;
        vulkanShaderProgramCreateContex.device = _Device;
        vulkanShaderProgramCreateContex.spvModuleSourceCode = std::vector<std::vector<char>>(shaderStageCount);
        vulkanShaderProgramCreateContex.modulesReflected = std::vector<SpvReflectShaderModule>(shaderStageCount);
        vulkanShaderProgramCreateContex.vkShaderModules = std::vector<vk::ShaderModule>(shaderStageCount);
        vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos = std::vector<vk::PipelineShaderStageCreateInfo>(
            shaderStageCount);

        for (size_t i = 0; i < shaderStageCount; i++)
            vulkanShaderProgramCreateContex.spvModuleSourceCode[i] = _programShaderCreateInfo[i]->GetCode();
        

        for (size_t i = 0; i < vulkanShaderProgramCreateContex.spvModuleSourceCode.size(); i++)
            spvReflectCreateShaderModule(vulkanShaderProgramCreateContex.spvModuleSourceCode[i].size(),
                vulkanShaderProgramCreateContex.spvModuleSourceCode[i].data(),
                &vulkanShaderProgramCreateContex.modulesReflected[i]);

        ParsePushConstantRange(vulkanShaderProgramCreateContex);

        // Create Modules
        for (size_t i = 0; i < vulkanShaderProgramCreateContex.spvModuleSourceCode.size(); i++)
        {
            vk::ShaderModuleCreateInfo createInfo = {};
            createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
            createInfo.codeSize = vulkanShaderProgramCreateContex.spvModuleSourceCode[i].size();
            createInfo.pCode = reinterpret_cast<uint32_t*>(vulkanShaderProgramCreateContex.spvModuleSourceCode[i].data());

            VK_CALL(
                vulkanShaderProgramCreateContex.device.createShaderModule(&createInfo, nullptr, &
                    vulkanShaderProgramCreateContex.vkShaderModules[i]));
        }

        // CreatePipelineShaderStage 
        for (size_t i = 0; i < vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos.size(); i++)
        {
            vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].sType =
                vk::StructureType::ePipelineShaderStageCreateInfo;
            vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].stage = static_cast<vk::ShaderStageFlagBits>(Utils::RhiToShaderStageBits(
                _programShaderCreateInfo[i]->GetShaderStageTypeBits()));
            vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].module = vulkanShaderProgramCreateContex.
                vkShaderModules[i];
            vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].pName = vulkanShaderProgramCreateContex.
                modulesReflected[i].entry_point_name;
        }

        return vulkanShaderProgramCreateContex;
	}


    void DestroyContext(VulkanShaderProgramCreateContext& _VulkanShaderProgramCreateContext)
    {
        for (auto& spvReflect : _VulkanShaderProgramCreateContext.modulesReflected)
            spvReflectDestroyShaderModule(&spvReflect);

        for (auto& vkShaderModule : _VulkanShaderProgramCreateContext.vkShaderModules)
            _VulkanShaderProgramCreateContext.device.destroyShaderModule(vkShaderModule);
    }

    vk::PipelineLayout Utils::CreatePipelineLayout(VulkanDescritptorManager& _VulkanDescritptorManager, vk::Device device, VulkanShaderProgramCreateContext& _VulkanShaderProgramCreateContext)
    {
        std::vector<vk::DescriptorSetLayout> cache = _VulkanDescritptorManager.GetDescriptorLayouts(_VulkanShaderProgramCreateContext.modulesReflected);

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
        if (!cache.empty() && cache[0] != VK_NULL_HANDLE)

        {
            pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(cache.size()); // Optional
            pipelineLayoutInfo.pSetLayouts = cache.data(); // Optional
        }

        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(_VulkanShaderProgramCreateContext.pushConstantRanges.size()); // Optional
        pipelineLayoutInfo.pPushConstantRanges = _VulkanShaderProgramCreateContext.pushConstantRanges.data(); // Optional

        return device.createPipelineLayout(pipelineLayoutInfo);
    }

} // namespace Utils

