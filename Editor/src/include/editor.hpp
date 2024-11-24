#pragma once

#include "app.hpp"
#include "dock_space.hpp"
#include "editor_header.hpp"
#include "editor_window.hpp"
#include "world/transform.hpp"
#include "reflection/reflector.hpp"

BEGIN_EDITOR_PCCORE


class Editor :  public PC_CORE::App
{
public:
    void Init() override;
    
    void Destroy() override;

    Editor() = default;

    ~Editor() = default;
    
    void InitTestScene();

    void DestroyTestScene();
    
    void Run() override;

    void InitEditorWindows();

    void UpdateEditorWindows();
    
    std::vector<EditorWindow*> m_EditorWindows;

    DockSpace dockSpace;
    
    PC_CORE::Entity* m_Selected = nullptr;
};

END_PCCORE