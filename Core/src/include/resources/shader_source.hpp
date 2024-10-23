#pragma once
#include "Resource.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"

BEGIN_PCCORE

class ShaderSource : public Resource
{
public:
    
    ShaderStageType shaderType;

    PC_CORE_API void SetPath(const fs::path& _path) override;

    std::vector<char> GetShaderSourceFile();

    void WriteFile(const fs::path& path) override;

    PC_CORE_API ~ShaderSource() override = default;

private:
    static std::vector<char> ReadFileAsChar(const std::string& _filename);
    
};

END_PCCORE