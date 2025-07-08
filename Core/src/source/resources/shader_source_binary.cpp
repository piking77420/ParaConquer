#include "resources/shader_source_binary.hpp"

#include "low_renderer/rhi_typedef.h"

PC_CORE::ShaderSourceBinary::ShaderSourceBinary(const std::string& _name, const std::vector<uint32_t>* _sprivCode) : Resource(_name), m_Path(SHADER_CACHE_PATH + name)
{
    DYNAMIC_REFLECT_INIT
    
    std::fstream f(m_Path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    
    if (!f.is_open())
    {
        PC_LOGERROR("File is not open {}", m_Path.generic_string());
        return;
    }

    // Write the contents of the vector to the file
    f.write(reinterpret_cast<const char*>(_sprivCode->data()), _sprivCode->size() * sizeof(uint32_t));

    f.close();
}

PC_CORE::ShaderSourceBinary::ShaderSourceBinary(std::string&& _name, const std::vector<uint32_t>* _sprivCode) : Resource(_name), m_Path(SHADER_CACHE_PATH + name)
{
    DYNAMIC_REFLECT_INIT
    
    std::fstream f(m_Path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    
    if (!f.is_open())
    {
        PC_LOGERROR("File is not open {}", m_Path.generic_string());
        return;
    }

    // Write the contents of the vector to the file
    f.write(reinterpret_cast<const char*>(_sprivCode->data()), _sprivCode->size() * sizeof(uint32_t));

    f.close();
}

