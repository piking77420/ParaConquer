#pragma once

#include <VulkanHeader.h>
#include <spirv_reflect.h>
#include <LowRenderer/RhiPipeline.hpp>

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


    class VULKAN_API VulkanPipeline : public PC_CORE::RhiPipeline
    {
    public:
        
        VulkanPipeline(PC_CORE::Rhi& _Rhi);

        ~VulkanPipeline() override;
        
        bool Build() override;
        
        vk::PipelineBindPoint GetPipelineBindPoint() const;

        vk::Pipeline GetPipeline() const;

        vk::PipelineLayout GetPipelineLayout() const;
    

    protected:
        
        size_t m_DescriptorId = std::numeric_limits<size_t>::max();

        vk::PipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

        vk::Pipeline m_Pipeline = VK_NULL_HANDLE;

        std::vector<vk::DynamicState> GetDynamicState() const;

        bool CreateFromContext(VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex);

        VulkanShaderProgramCreateContex CreateShaderProgramCreateContext(
            const std::vector<ShaderModule>& _programShaderCreateInfo, bool _createDescriptorResources = true);

        void CreatePipeLinePointGraphicsPipeline(const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex);

        void CreateComputePipeline(const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex);

        void CreatePipelineLayout(vk::Device _device,
                                  const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex);

#pragma region ParseRegion

        void ParseRasterizer(vk::PipelineRasterizationStateCreateInfo* _pipelineRasterizationStateCreateInfo);

        void ParsePipelineColorAttachementBlendState(
            vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState,
            const std::optional<RhiPipeline::BlendState>&);

        void ParsePipelineDepthStencilAttachmentState(
            vk::PipelineDepthStencilStateCreateInfo* _PipelineDepthStencilStateCreateInfo,
            const DephStencilInfo& _dephInfo);

        void ParseParsePipelineColorBlendState(vk::PipelineColorBlendStateCreateInfo* _PipelineColorBlendStateCreateInfo,
            const vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState,
            size_t _PipelineColorBlendAttachmentSize,
            const std::optional<RhiPipeline::BlendState>& _blendInfo);

        vk::VertexInputBindingDescription ParseVertexInputBindingDescription(
            const PC_CORE::VertexInputBindingDescrition& _vertexInputBindingDescrition);

        vk::VertexInputAttributeDescription ParseVertexInputAttributeDescription(
            const PC_CORE::VertexAttributeDescription& _vertexAttributeDescription);

        vk::PipelineVertexInputStateCreateInfo ParseVertexInputState( std::vector<vk::VertexInputBindingDescription>* _vertexInputBindingDescriptions
            , std::vector<vk::VertexInputAttributeDescription>* _vertexInputAttributeDescriptions);

        static void ParsePushConstantRange(VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex);

#pragma endregion ParseRegion

        void HotReload(const std::vector<ShaderModule>& _modules) override;

        
    };
}
