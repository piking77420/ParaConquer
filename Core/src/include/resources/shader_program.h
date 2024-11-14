#pragma once

#include "core_header.hpp"
#include "Resource.hpp"
#include "shader_source.hpp"
#include "rendering/render_harware_interface/descriptor_set.hpp"

constexpr uint32_t DESCRIPTOR_ALLOC_HIGH = 100;
constexpr uint32_t DESCRIPTOR_ALLOC_MEDIUM = 10;


BEGIN_PCCORE
    class ShaderProgram : public Resource
    {
    public:
        ShaderProgram(const ProgramShaderCreateInfo& _createInfo,
                      const std::vector<ShaderSource*>& _shaderSources);

        ShaderProgram() = default;

        ~ShaderProgram();

        void Reload(RenderPassHandle _renderPassHandle);

        void Bind(CommandBufferHandle _commandBuffer);
    
        void PushConstantMat4(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const Tbx::Matrix4x4f& m4);

        void PushConstantVec3(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const Tbx::Vector3f& vec3);

        void CreateDescriptorSet(uint32_t _descriptorSetLayout, PC_CORE::DescriptorSetHandle* _descriptorSet, uint32_t _descriptorSetCount);

        void FreeDescriptorSet(PC_CORE::DescriptorSetHandle* _descriptorSet, uint32_t _descriptorSetCount);

        void BindDescriptorSet(PC_CORE::CommandBufferHandle _commandBufferHandle, uint32_t _firstSet,
                               uint32_t _descriptorSetCount, const DescriptorSetHandle* _pDescriptorSets,
                               uint32_t _dynamicOffsetCount, const uint32_t* _pDynamicOffsets);
    
    private:
        ShaderInfo m_ShaderInfo;

        std::vector<ShaderSource*> m_ShaderSources;
    
        void CreateShader(RenderPassHandle _renderPassHandle);

        void DestroyShader();

        void PushConstant(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const  void* _data, size_t _dataSize);
    };

END_PCCORE
