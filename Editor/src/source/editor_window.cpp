#include "editor_window.hpp"	

#include "Imgui/imgui.h"

using namespace PC_EDITOR_CORE;

EditorWindow::EditorWindow(Editor& _editor) : m_Editor(&_editor)
{
    
}

void EditorWindow::Begin()
{
    ImGui::Begin(name.c_str());
}

void EditorWindow::End()
{
    ImGui::End();
}
