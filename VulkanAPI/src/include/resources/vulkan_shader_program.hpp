#pragma once

#include <vulkan_header.h>

#include "resources/shader_program.h"

struct SpvReflectShaderModule;

namespace Vulkan
{
    struct VulkanShaderProgramCreateContex
    {
        std::vector<std::vector<char>> spvModuleSourceCode;
        std::vector<SpvReflectShaderModule> modulesReflected;
        std::vector<vk::ShaderModule> vkShaderModules;
        std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;

        std::vector<vk::PushConstantRange> pushConstantRanges;


        vk::Device device;
    };

    struct PushConstantField
    {
        size_t pushConstantSize;
        size_t pushConstantOffSet;
        vk::ShaderStageFlags shaderStage;
    };
    

    constexpr uint32_t MAX_ALLOC_DESCRIPTOR_SET = 100 * MAX_FRAMES_IN_FLIGHT;
    
    class VulkanShaderProgram : public PC_CORE::ShaderProgram
    {
    protected:
        static constexpr  std::array<vk::DynamicState,9> dynamicStateArray =
       {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor,
            vk::DynamicState::eLineWidth,
            vk::DynamicState::eDepthBias,
            vk::DynamicState::eBlendConstants,
            vk::DynamicState::eDepthBounds,
            vk::DynamicState::eStencilCompareMask,
            vk::DynamicState::eStencilWriteMask,
            vk::DynamicState::eStencilReference
        };
    
    public:
        void AllocDescriptorSet(PC_CORE::ShaderProgramDescriptorSets** shaderProgramDescriptorSets, size_t set) override;
        
        void FreeDescriptorSet(PC_CORE::ShaderProgramDescriptorSets** shaderProgramDescriptorSets) override;

        void PushConstant(vk::CommandBuffer _commandBuffer, const std::string& _pushConstantKey, void* data, size_t _size) const;
        
        VulkanShaderProgram(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo);
    
        VulkanShaderProgram() = default;
    
        ~VulkanShaderProgram() override;

        vk::PipelineBindPoint GetPipelineBindPoint() const;

        vk::Pipeline GetPipeline() const;

        vk::PipelineLayout GetPipelineLayout() const;

    protected:

        size_t m_DescriptorSetAllocCount = 0;
        
        vk::PipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

        vk::Pipeline m_Pipeline = VK_NULL_HANDLE;

        std::vector<vk::DescriptorSetLayout> m_DescriptorSetLayout;

        vk::DescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

        std::unordered_map<std::string, PushConstantField> m_PushConstantMap;

        VulkanShaderProgramCreateContex CreateShaderProgramCreateContext(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo);
        
        void CreatePipeLinePointGraphicsPipeline(const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex, const PC_CORE::ShaderGraphicPointInfo& _shaderGraphicPointInf);

        void CreatePushConstantMapFromReflection(const std::vector<SpvReflectShaderModule>& _spvReflectShaderModule);

#pragma region ParseRegion
        void ParseSpvRelfection(VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContext);
        
        void ParseRasterizer(vk::PipelineRasterizationStateCreateInfo* _pipelineRasterizationStateCreateInfo, const PC_CORE::RasterizerInfo& _rasterizerInfo);

        void ParsePipelineColorBlendAttachmentState(vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState/*, const PC_CORE::*/);

        void ParseParsePipelineColorBlendAttachmentState(vk::PipelineColorBlendStateCreateInfo* _PipelineColorBlendStateCreateInfo, const vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState );
        
        vk::VertexInputBindingDescription ParseVertexInputBindingDescription(const PC_CORE::VertexInputBindingDescrition& _vertexInputBindingDescrition);

        vk::VertexInputAttributeDescription ParseVertexInputAttributeDescription(const PC_CORE::VertexAttributeDescription& _vertexAttributeDescription);

        vk::PipelineVertexInputStateCreateInfo ParseVertexInputState(
            const PC_CORE::ShaderGraphicPointInfo& _shaderGraphicPointInfo,
            std::vector<vk::VertexInputBindingDescription>* _vertexInputBindingDescriptions
            , std::vector<vk::VertexInputAttributeDescription>* _vertexInputAttributeDescriptions);

#pragma endregion ParseRegion 
       
    }; 
}


