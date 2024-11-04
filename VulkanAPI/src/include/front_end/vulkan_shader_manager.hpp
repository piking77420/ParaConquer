#pragma once

#include "rendering/render_harware_interface/rhi_typedef.h"
#include "vulkan_header.h"
#include <spirv_reflect.h>
#include <stack>


#include "vulkan_shader_compiler.hpp"


namespace Vulkan
{
    struct ShaderStageInfo
    {
        std::string name;
        PC_CORE::ShaderStageType shaderStage;
        SpvReflectShaderModule reflectShaderModule;
    };
      
    struct ReflectBlockVariable
    {
        vk::ShaderStageFlags stageFlags;
        std::string name;
        size_t size;
        size_t absoluteOffSet;
            
        std::vector<ReflectBlockVariable> members;
    };
        
    struct ShaderInternal
    {
        std::vector<ShaderStageInfo> shaderStages;
        std::vector<ReflectBlockVariable> reflectBlockVariables;
        std::vector<vk::DescriptorSet> descriptorsets;
        vk::DescriptorPool descriptorPool;

        vk::PipelineBindPoint pipelineBindPoint;
        vk::Pipeline pipeline = VK_NULL_HANDLE;
        vk::PipelineLayout pipelineLayout = VK_NULL_HANDLE;
        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    };

    
    class VulkanShaderManager
    {
    public:
        VULKAN_API bool CreateShaderFromSource(vk::Device _device, vk::RenderPass _tmprRenderPass ,const PC_CORE::ProgramShaderCreateInfo& _programShaderCreatInfo,
                                    const std::vector<PC_CORE::ShaderSourcePath>& _shaderSource);

        VULKAN_API bool DestroyShader(vk::Device _device, const std::string& _shaderName);
        

        VULKAN_API void BindProgram(vk::CommandBuffer _commandBuffer, const std::string& _shaderName);

        VULKAN_API void PushConstant(const std::string& _shaderName, const char* pushConstantName, const void* _value,
            size_t _size, vk::CommandBuffer _commandBuffer);

        VULKAN_API void Init(VulkanContext* _vulkanContext);

        VULKAN_API void Destroy(VulkanContext* _vulkanContext);

        VULKAN_API const ShaderInternal& GetShader(const std::string& _shaderName);

        VULKAN_API ShaderInternal* GetShaderInternal(const std::string& _shaderName);

    private:
        VulkanShaderCompiler m_ShaderCompiler;
        
        std::unordered_map<std::string, ShaderInternal> m_InternalShadersMap;

        static void ReflectMember(SpvReflectBlockVariable* spvReflectBlockVariable, ReflectBlockVariable* reflectBlockVariable, vk::ShaderStageFlags _stageFlags);
        
        static vk::ShaderStageFlagBits ShaderBitFromType(PC_CORE::ShaderStageType _shaderType);

        static void FillShaderInfo(ShaderInternal* _shaderInternalBack, const std::vector<PC_CORE::ShaderSourcePath>& _shaderSource);
        
        
        void DestroyInternalShaders(vk::Device _device, ShaderInternal* _shaderInternalBack);
        
        void CreatePipelineGraphicPointFromModule(vk::Device _device, vk::RenderPass _renderPass, const PC_CORE::ShaderInfo& ShaderInfo,
            const std::vector<vk::PipelineShaderStageCreateInfo>& _shaderStageCreateInfos, vk::PipelineLayout _pipelineLayout, vk::Pipeline* _outPipeline);

        void CreateShaderResourceFromSpvReflectModule(vk::Device _device, ShaderInternal* _shaderInternal);

        void ReflectPushConstantBlock(vk::Device _device, ShaderInternal* _shaderInternal, std::vector<vk::PushConstantRange>* _pushConstantRange);

        void RelflectDescriptorLayout(vk::Device _device, ShaderInternal* _shaderInternal, std::vector<vk::DescriptorSetLayoutBinding>* _DescriptorSetLayoutBindings);

        vk::PipelineVertexInputStateCreateInfo GetVertexInputStateCreateInfoFromShaderStruct(const PC_CORE::ShaderGraphicPointInfo& _shaderGraphicPointInfo, std::vector<vk::VertexInputBindingDescription>*
        _bindingDescriptions, std::vector<vk::VertexInputAttributeDescription>* _attributeDescriptions);

        void CreateDescriptorPool(vk::Device _device, ShaderInternal* _shaderInternal);

        void AllocDescriptorSet(vk::Device _device, ShaderInternal* _shaderInternal);
        
    };
}
