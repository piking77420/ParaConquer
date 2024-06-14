#pragma once

#include "app.hpp"
#include "editor_header.hpp"
#include "editor_window.hpp"

BEGIN_EDITOR_PCCORE
class Editor : public PC_CORE::App
{
public:
    void Init() override;
    
    void Destroy() override;

    Editor() = default;

    ~Editor() = default;
    
    void Run() override;

    void InitEditorWindows();

    std::vector<EditorWindow*> m_EditorWindows;
};

END_PCCORE