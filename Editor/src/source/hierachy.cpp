#include "hierachy.hpp"

#include "editor.hpp"
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
    PC_CORE::World& world = m_Editor->world;
    PC_CORE::Scene& scene = world.scene;


    /*
    std::vector<PC_CORE::Transform>& transforms = *scene.GetData<PC_CORE::Transform>();

    bool hasSelected = false;
    
    for (PC_CORE::Transform& transform : transforms)
    {
        PC_CORE::Entity* entity = scene.GetEntityFromId(transform.entityId);
        
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
    */
}
