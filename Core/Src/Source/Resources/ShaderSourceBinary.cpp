#include "Resources/ShaderSourceBinary.hpp"

#include <fstream>

#include "LowRenderer/Rhi.hpp"
#include "Io/inOut.h"


void PC_CORE::ShaderSourceBinary::OnParentReload(const Guid& _parentGuid)
{

}

void PC_CORE::ShaderSourceBinary::WriteSprivToFile(const std::vector<uint32_t>* _sprivCode, GraphicAPI _api)
{
    PERF_REGION_SCOPED;

    std::string binaryFolder;
    switch (_api)
    {
        break;
    case PC_CORE::GraphicAPI::Vulkan:
        binaryFolder = ShaderCacheVulkanFolder;
        break;
    case PC_CORE::GraphicAPI::D3d12:
        binaryFolder = ShaderCacheD3d12Folder;
        break;
    case PC_CORE::GraphicAPI::Count:
    case PC_CORE::GraphicAPI::None:
    default:
        assert(false);
        break;
    }

    std::filesystem::path parentPath = std::filesystem::path(SHADER_CACHE_PATH + binaryFolder);

    if (!std::filesystem::exists(parentPath))
    {
        std::filesystem::create_directories(parentPath);
    }

    std::string file = parentPath.generic_string() + name;
    if (PC_CORE::InOut::WriteFile(file, _sprivCode->data(), _sprivCode->size() * sizeof(uint32_t), true ))
    {
        m_Path = std::move(file);
    }
  
}

std::vector<char> PC_CORE::ShaderSourceBinary::GetCode() const
{
    if (m_Path.empty())
    {
        PC_LOGERROR("Can't get Code of ShaderSourceBinary because path is not initiliaze")
        return {};
    }
    
    std::vector<char> code;
    if (PC_CORE::InOut::ReadFile(m_Path, &code))
    {
        return code;
    }
    else
    {
        return {};
    }

    return {};
}


PC_CORE::ShaderSourceBinary::ShaderSourceBinary(const std::string& _name, const std::vector<uint32_t>* _sprivCode, ShaderStageType _shaderStageType, GraphicAPI _api) : Resource(_name), m_ShaderStageType(_shaderStageType)
{
    DYNAMIC_REFLECT_INIT
    WriteSprivToFile(_sprivCode, _api);
}

PC_CORE::ShaderSourceBinary::ShaderSourceBinary(std::string&& _name, const std::vector<uint32_t>* _sprivCode, ShaderStageType _shaderStageType, GraphicAPI _api) : Resource(std::move(_name)), m_ShaderStageType(_shaderStageType)
{
    DYNAMIC_REFLECT_INIT

    WriteSprivToFile(_sprivCode, _api);
}

