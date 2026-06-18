#include "Resources/ShaderSourceBinary.hpp"

#include <Io/FileLoader.hpp>
#include <LowRenderer/Rhi.hpp>

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
    case GraphicAPI::Vulkan:
        binaryFolder = ShaderCacheVulkanFolder;
        break;
    case GraphicAPI::D3d12:
        binaryFolder = ShaderCacheD3d12Folder;
        break;
    case GraphicAPI::Count:
    case GraphicAPI::None:
    default:
        assert(false);
        break;
    }

    auto parentPath = std::filesystem::path(ShaderCachePath + binaryFolder);

    if (!std::filesystem::exists(parentPath))
    {
        std::filesystem::create_directories(parentPath);
    }

    std::string file = parentPath.generic_string() + Name;
    if (FileLoader::WriteFile(file, _sprivCode->data(), _sprivCode->size() * sizeof(uint32_t), true))
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
    if (FileLoader::ReadFile(m_Path, &code))
    {
        return code;
    }

    return {};
}


PC_CORE::ShaderSourceBinary::ShaderSourceBinary(const std::string& _name, const std::vector<uint32_t>* _sprivCode,
            RhiShaderProgram::ShaderStageTypeBits _shaderStageType, GraphicAPI _api) : Resource(_name),
    m_ShaderStageType(_shaderStageType)
{
    DYNAMIC_REFLECT_INIT
    WriteSprivToFile(_sprivCode, _api);
}

PC_CORE::ShaderSourceBinary::ShaderSourceBinary(std::string&& _name, const std::vector<uint32_t>* _sprivCode,
    RhiShaderProgram::ShaderStageTypeBits _shaderStageType, GraphicAPI _api) : Resource(std::move(_name)),
                                                                   m_ShaderStageType(_shaderStageType)
{
    DYNAMIC_REFLECT_INIT

    WriteSprivToFile(_sprivCode, _api);
}
