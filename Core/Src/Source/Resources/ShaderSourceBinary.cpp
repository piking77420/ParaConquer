#include "Resources/ShaderSourceBinary.hpp"

#include <Fstream>

void PC_CORE::ShaderSourceBinary::OnParentReload(const Guid& _parentGuid)
{

}

void PC_CORE::ShaderSourceBinary::WriteSprivToFile(const std::vector<uint32_t>* _sprivCode)
{
    PERF_REGION_SCOPED;

    std::fstream f(m_Path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);

    if (!f.is_open())
    {
        PC_LOGERROR("File is not open {}", m_Path);
        return;
    }

    // Write the contents of the vector to the file
    f.write(reinterpret_cast<const char*>(_sprivCode->data()), _sprivCode->size() * sizeof(uint32_t));

    f.close();
}


PC_CORE::ShaderSourceBinary::ShaderSourceBinary(const std::string& _name, const std::vector<uint32_t>* _sprivCode, ShaderStageTypeFlag _shaderStageType) : Resource(_name), m_Path(SHADER_CACHE_PATH + name), m_ShaderStageType(_shaderStageType)
{
    DYNAMIC_REFLECT_INIT
    WriteSprivToFile(_sprivCode);
}

PC_CORE::ShaderSourceBinary::ShaderSourceBinary(std::string&& _name, const std::vector<uint32_t>* _sprivCode, ShaderStageTypeFlag _shaderStageType) : Resource(std::move(_name)), m_Path(SHADER_CACHE_PATH + name), m_ShaderStageType(_shaderStageType)
{
    DYNAMIC_REFLECT_INIT

    WriteSprivToFile(_sprivCode);
}

