#pragma once

#include "core_header.hpp"
#include "Resource.hpp"
#include "shader_source.hpp"

BEGIN_PCCORE
    class ShaderProgram : public Resource
    {
    public:
        ShaderProgram(const ProgramShaderCreateInfo& _createInfo,
                      const std::vector<ShaderSource*>& _shaderSources);

        ShaderProgram() = default;

        ~ShaderProgram();

        void Reload();

        void Bind(CommandBufferHandle _commandBuffer);
    
        void PushConstantMat4(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const Tbx::Matrix4x4f& m4);

        void PushConstantVec3(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const Tbx::Vector3f& vec3);

    private:
        ShaderInfo m_ShaderInfo;

        std::vector<ShaderSource*> m_ShaderSources;

        void CreateShader();

        void DestroyShader();

        void PushConstant(CommandBufferHandle _commandBuffer, const char* _pushConstantName, const  void* _data, size_t _dataSize);
    };

END_PCCORE
