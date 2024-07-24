#include "editor_window.hpp"	

#include "Imgui/imgui.h"

using namespace PC_EDITOR_CORE;



EditorWindow::EditorWindow(Editor& _editor, const std::string& _name) : m_Editor(&_editor) , name(_name)
{
}

void EditorWindow::Begin()
{
    ImGui::Begin(name.c_str(), &isOpen, windowFlags);
    GetInfo();
}

void EditorWindow::End()
{
    ImGui::End();
}

void EditorWindow::GetInfo()
{
    resize = false;
    ImVec2 imPos = ImGui::GetWindowPos();
    ImVec2 imSize = ImGui::GetWindowSize();

    Tbx::Vector2f newSize = { imSize.x , imSize.y};
    
    if (newSize != size)
        resize = true;
    
    position = { imPos.x , imPos.y};
    size = { imSize.x , imSize.y};

}
