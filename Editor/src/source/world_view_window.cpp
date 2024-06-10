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
    ShowViewPort();
}

void WorldViewWindow::ShowViewPort()
{
    ImGui::Begin("Viewport");

    

    ImGui::End();
}
