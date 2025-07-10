#pragma once

#include "rendering/shader_program.hpp"

BEGIN_PCCORE

struct GraphicShaderProgramCreateInfo
{
    ShaderGraphicPointInfo shaderGraphicPointInfo;
    SourceList sourceList;
    RhiRenderPass* renderPass;
};

class GraphicShader : public PC_CORE::ShaderProgram
{
public:

    IMP_DYNAMIC_REFLECT();

    PC_CORE_API GraphicShader(const std::string& _shaderName, const GraphicShaderProgramCreateInfo& _graphicShaderProgramCreateInfo);

    PC_CORE_API GraphicShader(std::string&& _shaderName, const GraphicShaderProgramCreateInfo& _graphicShaderProgramCreateInfo);
    
    PC_CORE_API GraphicShader()
    {
        DYNAMIC_REFLECT_INIT
    }

    PC_CORE_API virtual ~GraphicShader() = default;

private:

    
    REFLECT(GraphicShader, PC_CORE::ShaderProgram);
};

END_PCCORE