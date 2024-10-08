﻿#include "scene_button.hpp"

#include "editor.hpp"
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
    PC_CORE::World& world = m_Editor->world;

    const std::string buttonName = world.begin ? "Reset" : "Play";

    ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
    if (ImGui::Button(buttonName.c_str()))
    {
        if (!world.begin)
        {
            world.begin = true;
            world.run = true;
        }
        else
        {
            world.begin = false;
            world.run = false;
            m_Editor->DestroyTestScene();
            m_Editor->InitTestScene();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause"))
    {
        if (world.begin)
        {
            world.run = !world.run;
        }
    }
}
