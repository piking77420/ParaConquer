#pragma once

#include <string>

#include "EditorHeader.hpp"
#include "Rendering/RenderingTypedef.h"

BEGIN_EDITOR_PCCORE

constexpr const char* AssetsFormat = ".assets";
constexpr const char* AssetsRegisteryFileName = "AssetRegistry.json";


constexpr std::string_view EditorIniFileName = "PcEditor.ini";

struct EditorIniFile
{
    std::string projectPath;
    REFLECT(EditorIniFile);
    REFLECT_MEMBER(EditorIniFile, projectPath);
};

constexpr std::string_view ProjectFileName = "PcProject.ini";

struct ProjectFile
{
    std::string name;
    PC_CORE::GraphicAPI graphicApi;
    // to do version etc...

    REFLECT(ProjectFile);
    REFLECT_MEMBER(ProjectFile, name)
    REFLECT_MEMBER(ProjectFile, graphicApi)
};


END_EDITOR_PCCORE