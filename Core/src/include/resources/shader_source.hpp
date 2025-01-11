#pragma once
#include "resource.hpp"
#include "low_renderer/rhi_typedef.h"

BEGIN_PCCORE

class ShaderSource : public Resource
{
public:
    
    ShaderStageType shaderType;

    PC_CORE_API ShaderSource(const fs::path& _path);

    PC_CORE_API std::string GetShaderSourceFile();

    PC_CORE_API void WriteFile(const fs::path& path) override;

    PC_CORE_API ~ShaderSource() override = default;

private:
    static std::vector<char> ReadFileAsChar(const std::string& _filename);

    PC_CORE_API std::string IncludePath(const std::string& source, const std::filesystem::path& path);

    PC_CORE_API std::string GetFileAsString(const std::filesystem::path& path);
    
};

END_PCCORE