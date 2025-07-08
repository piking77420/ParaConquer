#pragma once

#include "resource.hpp"

BEGIN_PCCORE

class PC_CORE_API ShaderSourceBinary : public PC_CORE::Resource
{
public:

    const fs::path& GetPath()
    {
        return m_Path;
    }
    
    
    IMP_DYNAMIC_REFLECT()

    ShaderSourceBinary()
    {
        DYNAMIC_REFLECT_INIT;
    }

    ShaderSourceBinary(const std::string& _name, const std::vector<uint32_t>* _sprivCode);
    
    ShaderSourceBinary(std::string&& _name, const std::vector<uint32_t>* _sprivCode);
    
    ~ShaderSourceBinary() override = default;

private:
    fs::path m_Path;

    REFLECT(ShaderSourceBinary, PC_CORE::Resource);
    REFLECT_MEMBER(ShaderSourceBinary, m_Path);

};

END_PCCORE