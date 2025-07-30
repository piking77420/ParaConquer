#pragma once

#include "rendering/shader_program.hpp"

BEGIN_PCCORE

struct ComputeShaderProgramCreateInfo
{
    ShaderComputeInfo shaderComputeInfo;
    std::weak_ptr<ShaderSourceBinary> source;
};


class PC_CORE_API ComputeShader : public PC_CORE::ShaderProgram
{
public:

    IMP_DYNAMIC_REFLECT()

    ComputeShader(const std::string& _shaderName, const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo);

    ComputeShader(std::string&& _shaderName, const ComputeShaderProgramCreateInfo& _computeShaderProgramCreateInfo);

    ComputeShader()
    {
        DYNAMIC_REFLECT_INIT
    }

    ~ComputeShader() = default;
};

END_PCCORE