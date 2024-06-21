#include "scene_button.hpp"

#include "world/world.hpp"

using namespace PC_EDITOR_CORE;

SceneButton::SceneButton(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
    windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
}

void SceneButton::Update()
{
    EditorWindow::Update();

    OnEdit();
}

void SceneButton::OnEdit()
{
    PC_CORE::World* world = PC_CORE::World::world;
    
    ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
    if (ImGui::Button("Play"))
    {
        world->begin = true;
    }
    ImGui::SameLine();
    ImGui::Button("Reset");
}
