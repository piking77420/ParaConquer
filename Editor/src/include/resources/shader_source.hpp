#pragma once
#include <glslang/Include/glslang_c_interface.h>

#include "resources/resource.hpp"
#include "low_renderer/rhi_typedef.h"
#include "resources/shader_program.h"

BEGIN_PCCORE


class ShaderSource : public PC_CORE::ResourceInterface<PC_CORE::Resource>
{
public:
    ShaderSource(const fs::path& _path);
    
    void WriteFile(const fs::path& folder) override;

    void CompileToSpriv();
    
    ~ShaderSource() override = default;

    bool GetAsSpriv(std::vector<char>* _buffer);

private:
    ShaderStageType m_ShaderType;
    
    std::vector<char> IncludePath(const std::vector<char>& source, const std::filesystem::path& path);

    std::vector<char> GetShaderSourceFile();
};

END_PCCORE