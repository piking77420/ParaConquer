#pragma once

#include <VulkanHeader.h>

#include "LowRenderer/RhiShaderProgram.hpp"

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
    public:
        
        VulkanShaderProgram(PC_CORE::Rhi& _Rhi);

        ~VulkanShaderProgram() override;
        
        bool Build() override;
        
        const void* GetFrameNativeHandle(size_t _frameIndex) const override
        {
            return &m_Pipeline;
        }

        void* GetFrameNativeHandle(size_t _frameIndex) override
        {
            return &m_Pipeline; 
        }
        
        void PushConstant(vk::CommandBuffer _commandBuffer, const std::string& _pushConstantKey, const void* data,
                          size_t _size) const;
        
        vk::PipelineBindPoint GetPipelineBindPoint() const;

        vk::Pipeline GetPipeline() const;

        vk::PipelineLayout GetPipelineLayout() const;
    

    protected:
        static constexpr std::array<vk::DynamicState, 10> DynamicStateArray =
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
        
        size_t m_DescriptorId = std::numeric_limits<size_t>::max();

        vk::PipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

        vk::Pipeline m_Pipeline = VK_NULL_HANDLE;

        std::unordered_map<std::string, PushConstantField> m_PushConstantMap;

        bool CreateFromContext(VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex);

        VulkanShaderProgramCreateContex CreateShaderProgramCreateContext(
            const std::vector<ShaderModule>& _programShaderCreateInfo, bool _createDescriptorResources = true);

        void CreatePipeLinePointGraphicsPipeline(const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex);

        void CreateComputePipeline(const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex);

        void CreatePipelineLayout(vk::Device _device,
                                  const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex);

        void CreatePushConstantMapFromReflection(const std::vector<SpvReflectShaderModule>& _spvReflectShaderModule);

#pragma region ParseRegion
        void ParseDescriptor(VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContext);

        void ParseRasterizer(vk::PipelineRasterizationStateCreateInfo* _pipelineRasterizationStateCreateInfo);

        void ParsePipelineColorAttachementBlendState(
            vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState,
            const BlendState* _blendInfo);

        void ParsePipelineDepthStencilAttachmentState(
            vk::PipelineDepthStencilStateCreateInfo* _PipelineDepthStencilStateCreateInfo,
            const DephStencilInfo& _dephInfo);

        void ParseParsePipelineColorBlendState(vk::PipelineColorBlendStateCreateInfo* _PipelineColorBlendStateCreateInfo,
            const vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState,
            size_t _PipelineColorBlendAttachmentSize,
            const BlendState* _blendInfo);

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
