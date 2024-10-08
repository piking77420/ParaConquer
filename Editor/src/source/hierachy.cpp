﻿#include "hierachy.hpp"

#include "editor.hpp"
#include "Imgui/imgui.h"
#include "world/transform.hpp"
#include "world/world.hpp"

using namespace PC_EDITOR_CORE;

Hierachy::Hierachy(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
    
}

void Hierachy::Update()
{
    EditorWindow::Update();
    ShowGraph();
}

void Hierachy::ShowGraph()
{
    
    if (PC_CORE::World::world == nullptr)
        return;
    
    PC_CORE::World& world = *PC_CORE::World::world;
    const PC_CORE::Scene& scene = world.scene;

    const std::vector<PC_CORE::Transform>* transforms = nullptr;
    scene.GetComponentData<PC_CORE::Transform>(&transforms);

    bool hasSelected = false;
    
    for (size_t i = 0 ; i < transforms->size(); i++)
    {
        if (!PC_CORE::IsValid(transforms->at(i).componentHolder))
            continue;

        auto entityInternal = world.scene.GetEntityInternal(transforms->at(i).componentHolder.entityID);
            
        if (ImGui::Button(entityInternal->name.c_str()))
        {
            m_Editor->selected = static_cast<uint32_t>(i);
            hasSelected = true;
        }
        
    }

    if (!hasSelected)
    {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsWindowFocused())
        {
            m_Editor->selected = NULL_ENTITY;
        }
    }
    
}
