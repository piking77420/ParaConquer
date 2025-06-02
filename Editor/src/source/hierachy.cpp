#include "hierachy.hpp"

#include "editor.hpp"
#include "world/transform.hpp"
#include "world/world.hpp"
#include "ecs/entity_manager.h"

using namespace PC_EDITOR_CORE;

Hierachy::Hierachy(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
    PC_CORE::Reflector::GetPtrToTypeField<PC_CORE::World, PC_CORE::EntityManager>(&PC_CORE::App::instance->world, "m_EntityManager", &m_EntityManagerPtr);
    
    PC_CORE::Reflector::GetPtrToTypeField<PC_CORE::EntityManager, std::bitset<PC_CORE::MAX_ENTITIES>>(m_EntityManagerPtr, "m_EntityEnableFlags", &m_EnableEntitiesBitSetPtr);
}

void Hierachy::Update()
{
    EditorWindow::Update();
    ShowGraph();
}

void Hierachy::ShowGraph()
{
    if (m_EntityManagerPtr == nullptr)
    {
        PC_LOGERROR("m_EntityManagerPtr is nullPtr");
        return;
    }
    
    
    if (m_EnableEntitiesBitSetPtr == nullptr)
    {
        PC_LOGERROR("m_EnableEntitiesBitSetPtr is nullPtr");
        return;
    }
    
    
    bool hasSelected = false;

    for (size_t i = 0 ; i < m_EnableEntitiesBitSetPtr->size(); i++)
    {
        if (!m_EnableEntitiesBitSetPtr->test(i))
            continue;
        
        PC_CORE::EntityId id = static_cast<PC_CORE::EntityId>(i);
        auto entName = m_EntityManagerPtr->GetEntityName(i);
        

        if (ImGui::Button(entName.data()))
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
