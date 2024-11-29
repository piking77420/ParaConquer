#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "shader_source.hpp"
#include "rendering/render_harware_interface/descriptor_set.hpp"

constexpr uint32_t DESCRIPTOR_ALLOC_HIGH = 100;
constexpr uint32_t DESCRIPTOR_ALLOC_MEDIUM = 10;


BEGIN_PCCORE
    class ShaderProgram : public Resource
    {
    public:

        PC_CORE_API ShaderProgram(const ProgramShaderCreateInfo& _createInfo,
                      const std::vector<ShaderSource*>& _shaderSources);

        PC_CORE_API ShaderProgram() = default;

        PC_CORE_API ~ShaderProgram();

        PC_CORE_API void Reload(RenderPassHandle _renderPassHandle);

        PC_CORE_API void Bind(CommandBufferHandle _commandBuffer);

        PC_CORE_API void PushConstantMat4(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const Tbx::Matrix4x4f& m4);

        PC_CORE_API void PushConstantVec3(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const Tbx::Vector3f& vec3);

        PC_CORE_API void PushConstantVec4(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const Tbx::Vector4f& vec4);

        PC_CORE_API void PushConstant(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const  void* _data, size_t _dataSize);

        PC_CORE_API void CreateDescriptorSet(uint32_t _descriptorSetLayout, PC_CORE::DescriptorSetHandle* _descriptorSet, uint32_t _descriptorSetCount);

        PC_CORE_API void FreeDescriptorSet(PC_CORE::DescriptorSetHandle* _descriptorSet, uint32_t _descriptorSetCount);

        PC_CORE_API void BindDescriptorSet(PC_CORE::CommandBufferHandle _commandBufferHandle, uint32_t _firstSet,
                               uint32_t _descriptorSetCount, const DescriptorSetHandle* _pDescriptorSets,
                               uint32_t _dynamicOffsetCount, const uint32_t* _pDynamicOffsets);

        
    
    private:
        ShaderInfo m_ShaderInfo;

        std::vector<ShaderSource*> m_ShaderSources;
    
        PC_CORE_API void CreateShader(RenderPassHandle _renderPassHandle);

        PC_CORE_API void DestroyShader();

    };

END_PCCORE
