#pragma once

#include <vulkan_header.h>

#include "low_renderer/rhi_shader_program.hpp"

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
    

    
    class VULKAN_API VulkanShaderProgram : public PC_CORE::RhiShaderProgram
    {
    protected:
        static constexpr  std::array<vk::DynamicState,10> dynamicStateArray =
       {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor,
            vk::DynamicState::eLineWidth,
            vk::DynamicState::eDepthBias,
            vk::DynamicState::eDepthBounds,
            vk::DynamicState::eStencilCompareMask,
            vk::DynamicState::eStencilWriteMask,
            vk::DynamicState::eStencilReference,
            vk::DynamicState::ePrimitiveTopology,
            vk::DynamicState::eBlendConstants,
        };
    
    public:
        void AllocDescriptorSet(PC_CORE::ShaderProgramDescriptorSets** shaderProgramDescriptorSets, size_t set) override;
        
        void FreeDescriptorSet(PC_CORE::ShaderProgramDescriptorSets** shaderProgramDescriptorSets) override;

        void PushConstant(vk::CommandBuffer _commandBuffer, const std::string& _pushConstantKey, const void* data, size_t _size) const;
        
        VulkanShaderProgram(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo);
    
        VulkanShaderProgram() = default;
    
        ~VulkanShaderProgram() override;

        vk::PipelineBindPoint GetPipelineBindPoint() const;

        vk::Pipeline GetPipeline() const;

        vk::PipelineLayout GetPipelineLayout() const;

        const void* GetNativeHandle() const override
        {
            return &m_Pipeline;
        }
        void* GetNativeHandle() override
        {
            return &m_Pipeline;
        }
        

    protected:

        size_t m_DescriptorId = std::numeric_limits<size_t>::max();

        size_t m_DescriptorSetAllocCount = 0;
        
        vk::PipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

        vk::Pipeline m_Pipeline = VK_NULL_HANDLE;
        
        std::unordered_map<std::string, PushConstantField> m_PushConstantMap;

        VulkanShaderProgramCreateContex CreateShaderProgramCreateContext(const std::vector<std::pair<PC_CORE::ShaderStageTypeFlag, std::string>>& _programShaderCreateInfo, bool _createDescriptorResources = true);
        
        void CreatePipeLinePointGraphicsPipeline(const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex, const PC_CORE::ShaderGraphicPointInfo& _shaderGraphicPointInf);

        void CreateComputePipeline(const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex, const PC_CORE::ShaderComputeInfo& _shaderComputeInfo);

        void CreatePushConstantMapFromReflection(const std::vector<SpvReflectShaderModule>& _spvReflectShaderModule);

#pragma region ParseRegion
        void ParseDescriptor(VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContext);
        
        void ParseRasterizer(vk::PipelineRasterizationStateCreateInfo* _pipelineRasterizationStateCreateInfo, const PC_CORE::RasterizerInfo& _rasterizerInfo);

        void ParsePipelineColorAttachementBlendState(vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState, const PC_CORE::BlendInfo* _blendInfo);

        void ParsePipelineDepthStencilAttachmentState(vk::PipelineDepthStencilStateCreateInfo* _PipelineDepthStencilStateCreateInfo, const PC_CORE::DephStencilInfo& _dephInfo);

        void ParseParsePipelineColorBlendState(vk::PipelineColorBlendStateCreateInfo* _PipelineColorBlendStateCreateInfo, const vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState , size_t _PipelineColorBlendAttachmentSize,
            const PC_CORE::BlendInfo* _blendInfo);
        
        vk::VertexInputBindingDescription ParseVertexInputBindingDescription(const PC_CORE::VertexInputBindingDescrition& _vertexInputBindingDescrition);

        vk::VertexInputAttributeDescription ParseVertexInputAttributeDescription(const PC_CORE::VertexAttributeDescription& _vertexAttributeDescription);

        vk::PipelineVertexInputStateCreateInfo ParseVertexInputState(
            const PC_CORE::ShaderGraphicPointInfo& _shaderGraphicPointInfo,
            std::vector<vk::VertexInputBindingDescription>* _vertexInputBindingDescriptions
            , std::vector<vk::VertexInputAttributeDescription>* _vertexInputAttributeDescriptions);

        void ParsePushConstantRange(VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex);

#pragma endregion ParseRegion 
      
        void HotReload(const std::vector<std::pair<PC_CORE::ShaderStageTypeFlag, std::string>>& _sources) override;

    };
}


