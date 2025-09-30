#pragma once

#include "Resource.hpp"
#include "LowRenderer/RhiTypedef.h"

BEGIN_PCCORE

class PC_CORE_API ShaderSourceBinary : public PC_CORE::Resource
{
public:

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

    void WriteSprivToFile(const std::vector<uint32_t>* _sprivCode, GraphicAPI _api);

    std::vector<char> GetCode() const;

    ShaderSourceBinary(const std::string& _name, const std::vector<uint32_t>* _sprivCode, ShaderStageType _shaderStageType, GraphicAPI _api);
    
    ShaderSourceBinary(std::string&& _name, const std::vector<uint32_t>* _sprivCode, ShaderStageType _shaderStageType, GraphicAPI _api);
    
    ~ShaderSourceBinary() override = default;

private:
    ShaderStageType m_ShaderStageType;

    std::string m_Path;

    REFLECT(ShaderSourceBinary, PC_CORE::Resource);

};

END_PCCORE