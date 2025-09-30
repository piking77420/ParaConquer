#pragma once

#include "EditorFiles.hpp"
#include "EditorHeader.hpp"

BEGIN_EDITOR_PCCORE

class ProjectMaker
{
public:
    ProjectMaker() = default;

    ~ProjectMaker() = default;

    static ProjectFile CreateBaseProject(const wchar_t* _path);
    
private:
    
};

END_EDITOR_PCCORE