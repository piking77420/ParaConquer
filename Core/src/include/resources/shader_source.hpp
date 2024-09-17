#pragma once
#include "resource.hpp"
#include "rendering/rhi_typedef.h"

BEGIN_PCCORE

class ShaderSource : public IResource
{
public:
    
    LowLevelShaderStageType shaderType;

    PC_CORE_API void SetPath(const fs::path& _path) override;

    std::vector<uint8_t> GetData() override;

    void WriteFile(const fs::path& path) override;

    PC_CORE_API ~ShaderSource() override;

private:
    static std::vector<uint8_t> ReadFile(const std::string& _filename);

};

END_PCCORE