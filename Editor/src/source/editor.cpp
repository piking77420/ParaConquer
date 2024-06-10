#include "editor.hpp"

#include "edit_world_window.hpp"
#include "world_view_window.hpp"
#include "Imgui/imgui.h"
#include "resources/resource_manager.hpp"

namespace PC_CORE
{
    struct SphereCollider;
}

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;

void Editor::Init()
{
    App::Init();
    InitEditorWindows();
}

void Editor::Destroy()
{
    for (const EditorWindow* editorWindow : m_EditorWindows)
        delete editorWindow;
    
    App::Destroy();
}



void Editor::Run()
{
    while (!windowHandle.ShouldClose())
    {
        windowHandle.PoolEvents();
        HandleResize();
        vulkanImgui.NewFrame();
        MoveObject();
        renderer.BeginFrame();

        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Begin();
            editorWindow->Update();
            editorWindow->End();
            
        }
        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Render();
        }
       
        renderer.SwapBuffers();
    }   
}

void Editor::InitEditorWindows()
{
    m_EditorWindows.push_back(new EditWorldWindow(*this));
    m_EditorWindows[0]->name = "EditorWindow";
}

