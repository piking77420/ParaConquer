#include "EditorWindow.hpp"

#include "Editor.hpp"


using namespace PC_EDITOR_CORE;


EditorWindow::EditorWindow(Editor& _editor, const std::string& _name) : windowFlags(0), name(_name), m_Editor(&_editor)
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
    ImVec2 imPos = ImGui::GetWindowPos();
    ImVec2 imSize = ImGui::GetWindowSize();

    Tbx::Vector2f newSize = {imSize.x, imSize.y};
    resize = (newSize != size);

    size = {imSize.x, imSize.y};
    position = {imPos.x, imPos.y};
}

bool EditorWindow::IsInsideWindow(Tbx::Vector2f _point) const
{
    const bool insideX = (_point.x >= position.x) && (_point.x <= (position.x + size.x));
    const bool insideY = (_point.y >= position.y) && (_point.y <= (position.y + size.y));

    return insideX && insideY;
}

bool EditorWindow::IsCursorInsideWindow() const
{
    const auto cursorPos = m_Editor->gameApp.MainWindow.GetCursorPos();


    return IsInsideWindow({static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y)});
}
