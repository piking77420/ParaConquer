#include "world_view_window.hpp"

#include "Imgui/imgui.h"

using namespace PC_EDITOR_CORE; 

WorldViewWindow::WorldViewWindow(Editor& _editor)
: EditorWindow(_editor)
{
}


void WorldViewWindow::Update()
{
    EditorWindow::Update();

    const Vector2i windowSize = { static_cast<int32_t>(ImGui::GetWindowWidth()) ,
        static_cast<int32_t>(ImGui::GetWindowHeight()) };
    
    viewport.HandleResize(windowSize);
    ShowViewPort();
}

void WorldViewWindow::ShowViewPort()
{
    
}
