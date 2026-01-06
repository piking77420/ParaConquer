#include "ProjectMaker.hpp"

#include <filesystem>
#include <fstream>

#include "EditorFiles.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "Serialize/Serializer.h"
#include "Serialize/JsonSerializer.hpp"

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


    PC_CORE::JsonSerializer s;

    s.Serialize<ProjectFile>(projectFile, (path / ProjectFileName).generic_string());

    return std::move(projectFile);
}
