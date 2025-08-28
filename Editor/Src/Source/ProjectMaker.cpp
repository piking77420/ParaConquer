#include "ProjectMaker.hpp"

#include <filesystem>
#include <fstream>

#include "EditorFiles.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "Serialize/Serializer.h"

PC_EDITOR_CORE::ProjectFile PC_EDITOR_CORE::ProjectMaker::CreateBaseProject(const wchar_t* _path)
{
    const std::filesystem::path path(_path);
    std::filesystem::create_directory(path);
    std::filesystem::create_directory(path / "/Assets");
    
    const ProjectFile projectFile =
        {
        .name = path.filename().string(),
        .graphicApi = PC_CORE::GraphicAPI::Vulkan
        };

    PC_CORE::Serializer::Serialize(projectFile, (path / ProjectFileName).generic_string());

    return std::move(projectFile);
}
