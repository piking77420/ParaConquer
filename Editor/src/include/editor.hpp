#pragma once

#include "app.hpp"
#include "dock_space.hpp"
#include "editor_header.hpp"
#include "editor_window.hpp"
#include "io/imgui_context.h"
#include "physics/rigid_body.hpp"
#include "world/transform.hpp"
#include "reflection/reflector.hpp"

BEGIN_EDITOR_PCCORE


class Editor
{
public:
    void Init();
    
    void Destroy();

    Editor();

    ~Editor();
    
    void InitTestScene();
    
    void DestroyTestScene();
    
    void Run(bool* _appShouldClose);

    void InitEditorWindows();

    void UpdateEditorWindows();

    PC_CORE::App gameApp;
    
    std::vector<EditorWindow*> m_EditorWindows;

    DockSpace dockSpace;
    
    PC_CORE::EntityId m_SelectedEntityId = PC_CORE::INVALID_ENTITY_ID;

    PC_CORE::IMGUIContext IMGUIContext;

private:
    void InitThridPartLib();

    void UnInitThridPartLib();

    void CompileShader();
};

END_PCCORE