#pragma once

#include "Rendering/ShaderProgram.hpp"

BEGIN_PCCORE

    struct ComputeShaderProgramCreateInfo
    {
        RhiShaderProgram::ShaderComputeInfo shaderComputeInfo;
        std::weak_ptr<ShaderSourceBinary> source;
    };


    class PC_CORE_API ComputeShader : public ShaderProgram
    {
    public:
        const RhiShaderProgram::LocalSize& GetLocalSize() const
        {
            return m_RhiShaderProgram->GetLocalSize();
        }

        IMP_DYNAMIC_REFLECT()

        ComputeShader(Rhi& rhi, const std::string& _shaderName,
                      const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo);

        ComputeShader(Rhi& rhi, std::string&& _shaderName, const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo);

        ComputeShader()
        {
            DYNAMIC_REFLECT_INIT
        }

        ~ComputeShader() override = default;

        RhiShaderProgram* const Get() const
        {
            return m_RhiShaderProgram.get();
        }

        RhiShaderProgram* const operator->() const
        {
            return m_RhiShaderProgram.operator->();
        }

    };


    REFLECT(ComputeShader, PC_CORE::ShaderProgram);

END_PCCORE
