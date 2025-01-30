#pragma once
#include <glslang/Include/glslang_c_interface.h>

#include "resources/resource.hpp"
#include "low_renderer/rhi_typedef.h"

BEGIN_PCCORE


class ShaderSource : public PC_CORE::Resource
{
public:
    ShaderSource(const fs::path& _path);
    
    void WriteFile(const fs::path& folder) override;

    void CompileToSpriv();
    
    ~ShaderSource() override;

    bool GetAsSpriv(std::vector<char>* _buffer);

private:
    ShaderStageType m_ShaderType;
    
    std::vector<char> IncludePath(const std::vector<char>& source, const std::filesystem::path& path);

    std::vector<char> GetShaderSourceFile();

    void SerializeModule(std::vector<char>* _spvCode);
};

END_PCCORE