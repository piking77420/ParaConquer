#pragma once

#include "resource.hpp"
#include "low_renderer/rhi_typedef.h"

BEGIN_PCCORE

class PC_CORE_API ShaderSourceBinary : public PC_CORE::Resource
{
public:

    const fs::path& GetPath()
    {
        return m_Path;
    }

    ShaderStageType GetShaderStageType() const
    {
        return m_ShaderStageType;
    }
       
    IMP_DYNAMIC_REFLECT()

    ShaderSourceBinary()
    {
        DYNAMIC_REFLECT_INIT;
    }

    void OnParentReload(const Guid& _parentGuid) override;

    void WriteSprivToFile(const std::vector<uint32_t>* _sprivCode);

    ShaderSourceBinary(const std::string& _name, const std::vector<uint32_t>* _sprivCode, ShaderStageType _shaderStageType);
    
    ShaderSourceBinary(std::string&& _name, const std::vector<uint32_t>* _sprivCode, ShaderStageType _shaderStageType);
    
    ~ShaderSourceBinary() override = default;

private:
    fs::path m_Path;

    ShaderStageType m_ShaderStageType;

    REFLECT(ShaderSourceBinary, PC_CORE::Resource);
    REFLECT_MEMBER(ShaderSourceBinary, m_Path);

};

END_PCCORE