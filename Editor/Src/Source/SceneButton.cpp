#include "SceneButton.hpp"

#include "Editor.hpp"
#include "World/World.hpp"

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
    PC_CORE::World& world = m_Editor->gameApp.world;

    const std::string buttonName = world.run ? "Reset" : "Play";
    
    ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
    if (ImGui::Button(buttonName.c_str()))
    {
        if (!world.run)
        {
            world.begin = true;
            for (auto& it : m_Editor->editorWindows)
                it->OnPlayButton();
        }
        else
        {
            world.begin = false;
            world.run = false;
            for (auto& it : m_Editor->editorWindows)
                it->OnResetScene();
        }
    }
    const std::string buttonName2 = world.run ? "Pause" : "Resume";

    ImGui::SameLine();
    if (ImGui::Button(buttonName2.c_str()))
    {
        world.run = !world.run;
    }
}
