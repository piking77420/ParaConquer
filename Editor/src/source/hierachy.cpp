#include "hierachy.hpp"

#include "editor.hpp"
#include "world/transform.hpp"
#include "world/world.hpp"
#include "ecs/entity_manager.h"

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
    PC_CORE::EntityManager& ent = PC_CORE::World::GetWorld()->entityManager;
    
    bool hasSelected = false;

    size_t entityCapacity = ent.GetEntityCapacity();
    for (size_t i = 0 ; i < entityCapacity; i++)
    {
        PC_CORE::EntityId id = static_cast<PC_CORE::EntityId>(i);
        if (!ent.IsValidEntityId(id))
            return;
        
        PC_CORE::Entity& entity = ent.GetEntity(id);
        if (ImGui::Button(entity.name.c_str()))
        {
            m_Editor->m_SelectedEntityId = id;
            hasSelected = true;
        }
    }

    if (!hasSelected)
    {
        
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsWindowFocused())
        {
            m_Editor->m_SelectedEntityId = PC_CORE::INVALID_ENTITY_ID;
        }
    }
    
}
