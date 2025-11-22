#pragma once

#include "Rendering/ShaderProgram.hpp"

BEGIN_PCCORE
    struct ComputeShaderProgramCreateInfo
    {
        ShaderComputeInfo shaderComputeInfo;
        std::weak_ptr<ShaderSourceBinary> source;
    };


    class PC_CORE_API ComputeShader : public ShaderProgram
    {
    public:
        const LocalSize& GetLocalSize() const
        {
            return m_RhiShaderProgram->GetLocalSize();
        }

        IMP_DYNAMIC_REFLECT()

        ComputeShader(const std::string& _shaderName,
                      const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo);

        ComputeShader(std::string&& _shaderName, const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo);

        ComputeShader()
        {
            DYNAMIC_REFLECT_INIT
        }

        ~ComputeShader() override = default;
    };


    REFLECT(ComputeShader, PC_CORE::ShaderProgram);

END_PCCORE
