#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "shader_source.hpp"

BEGIN_PCCORE
    class ShaderProgram : public IResource
{
public:

    ShaderProgram(const ProgramShaderCreateInfo& _createInfo,
        const std::vector<ShaderSource*>& _shaderSources);

    ShaderProgram() = default;

    ~ShaderProgram();

    void Reload();

    void Bind(CommandBufferHandle _commandBuffer);

    void PushVector3(CommandBufferHandle _commandBuffer, const char* _name, void* _data);

private:

    void CreateShader();

    void DestroyShader();
    
    ShaderInfo m_ShaderInfo;

    std::vector<ShaderSource*> m_ShaderSources;
};

END_PCCORE