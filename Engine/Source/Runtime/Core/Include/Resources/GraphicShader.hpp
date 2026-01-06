#pragma once

#include "Rendering/ShaderProgram.hpp"

BEGIN_PCCORE
/*
    struct GraphicShaderProgramCreateInfo
    {
        RhiShaderProgram::ShaderGraphicPointInfo shaderGraphicPointInfo;
        SourceList sourceList;
        RhiRenderPass* renderPass;
        uint32_t colorAttachementCount;
        uint32_t subPassIndex;
    };

    class GraphicShader : public ShaderProgram
    {
    public:
        IMP_DYNAMIC_REFLECT();

        PC_CORE_API GraphicShader(Rhi& rhi, const std::string& _shaderName,
                                  const GraphicShaderProgramCreateInfo& _graphicShaderProgramCreateInfo);

        PC_CORE_API GraphicShader(Rhi& rhi, std::string&& _shaderName,
                                  const GraphicShaderProgramCreateInfo& _graphicShaderProgramCreateInfo);

        PC_CORE_API GraphicShader()
        {
            DYNAMIC_REFLECT_INIT
        }

        PC_CORE_API ~GraphicShader() override = default;

        RhiShaderProgram* const Get() const
        {
            return m_RhiShaderProgram.get();
        }

        RhiShaderProgram* const operator->() const
        {
            return m_RhiShaderProgram.operator->();
        }


    private:
        REFLECT(GraphicShader, PC_CORE::ShaderProgram);
    };
    */
END_PCCORE
