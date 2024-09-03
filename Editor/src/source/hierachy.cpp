#include "hierachy.hpp"

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
    // TODO Update ECS
    
    if (PC_CORE::World::world == nullptr)
        return;
    
    PC_CORE::World& world = *PC_CORE::World::world;
    PC_CORE::Scene& scene = world.scene;

    size_t nbr = 0;
    const uint8_t* transformData = scene.GetData<PC_CORE::Transform>(&nbr);

    bool hasSelected = false;
    
    for (size_t i = 0; i < nbr; i++)
    {
        const PC_CORE::Transform* transform = reinterpret_cast<const PC_CORE::Transform*>(transformData + sizeof(PC_CORE::Transform) * i);
        PC_CORE::Entity* entity = scene.GetEntity(transform->entityId);
        
        if (ImGui::Button(entity->name.c_str()))
        {
            m_Editor->m_Selected = entity;
            hasSelected = true;
        }
    }

    if (!hasSelected)
    {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsWindowFocused())
        {
            m_Editor->m_Selected = nullptr;
        }
    }
    
}
