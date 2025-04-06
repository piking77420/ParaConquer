#pragma once

#include "resources/resource.hpp"
#include "low_renderer/rhi_typedef.h"
#include "resources/shader_program.h"

BEGIN_PCCORE


class ShaderSource : public PC_CORE::ResourceInterface<ShaderSource>
{
public:
    ShaderSource();

    ShaderSource(const fs::path& _path);
    
    void CompileToSpriv();
    
    ~ShaderSource() override = default;

    bool GetAsSpriv(std::vector<char>* _buffer);

private:
    ShaderStageType m_ShaderType;

    fs::path m_PathToSource;
    
    std::vector<char> IncludePath(const std::vector<char>& source, const std::filesystem::path& path);

    std::vector<char> GetShaderSourceFile();
};

END_PCCORE