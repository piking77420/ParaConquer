#include "hierachy.hpp"

#include "editor.hpp"
#include "command/editor_command_create_entity.hpp"
#include "world/transform.hpp"
#include "world/world.hpp"
#include "ecs/entity_manager.h"

using namespace PC_EDITOR_CORE;

Hierachy::Hierachy(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
    PC_CORE::Reflector::GetPtrToTypeField<PC_CORE::Level, PC_CORE::EntityManager>(&PC_CORE::App::instance->world.level, "m_EntityManager", &m_EntityManagerPtr);
    
    PC_CORE::Reflector::GetPtrToTypeField<PC_CORE::EntityManager, std::bitset<PC_CORE::MAX_ENTITIES>>(m_EntityManagerPtr, "m_EntityEnableFlags", &m_EnableEntitiesBitSetPtr);
}


void Hierachy::Update()
{
    PERF_REGION_SCOPED;
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
    
    
    // TODO test multiple bytes at once 
    bool hasSelected = false;
    const size_t byteCount = m_EnableEntitiesBitSetPtr->size();

    std::bitset<PC_CORE::MAX_ENTITIES> bitsetIterator = *m_EnableEntitiesBitSetPtr;
    for (size_t i = 0 ; i < byteCount; i++)
    {
        if (bitsetIterator == 0)
            break;

        if (m_EnableEntitiesBitSetPtr->test(i))
        {
            PC_CORE::EntityId id = static_cast<PC_CORE::EntityId>(i);
            auto entName = m_EntityManagerPtr->GetEntityName(i);

            if (ImGui::Button(entName.data()))
            {
                m_Editor->selectedObject = id;
                hasSelected = true;
            }
        }
        bitsetIterator.set(i, false);
    }


    if (!hasSelected)
    {
        
        if (ImGui::IsWindowFocused() && IsCursorInsideWindow() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            m_Editor->selectedObject = std::monostate();
        }
    }

    if (IsCursorInsideWindow() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        ImGui::OpenPopup("HierarchyAction", 0);

    
    if (ImGui::BeginPopup("HierarchyAction"))
    {
        if (ImGui::Selectable("CreateEntity"))
        {
            m_Editor->PushCommand<EditorCommandCreateEntity>();
        }
        ImGui::EndPopup();
    }

   
    
    
}
