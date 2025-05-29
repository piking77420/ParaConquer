#include "editor_window.hpp"	


using namespace PC_EDITOR_CORE;



EditorWindow::EditorWindow(Editor& _editor, const std::string& _name) : m_Editor(&_editor), name(_name), windowFlags(0)
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
    
    size = { imSize.x , imSize.y};
    position = { imPos.x , imPos.y};

    position.x *= 0.5f * size.x;
    position.y *= 0.5f * size.y;
}

bool EditorWindow::IsInsideWindow(Tbx::Vector2f _point) const
{
    bool outsideX = _point.x > (position.x + size.x) && _point.x < (position.x - size.x);
    bool outsideY = _point.y > (position.y + size.y) && _point.y < (position.y - size.y);

    return !(outsideX || outsideY);
}
