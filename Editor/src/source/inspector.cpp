#include "inspector.hpp"

#include "editor.hpp"
#include "imgui_helper.h"

using namespace PC_EDITOR_CORE;

void Inspector::Update()
{
    EditorWindow::Update();
    
    if(m_Editor->selected == NULL_ENTITY)
        return;

    Show();
    OnInput();
    
}

Inspector::Inspector(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
}

void Inspector::Show()
{
    PC_CORE::Transform* transform = PC_CORE::World::world->scene.GetComponent<PC_CORE::Transform>(m_Editor->selected);
    if (transform == nullptr)
        return;
    const auto entityInternal = PC_CORE::World::world->scene.GetEntityInternal(transform->componentHolder.entityID);
    ImGui::PushID(transform->componentHolder.entityID);
    ImGui::Text(("entity " + entityInternal->name).c_str());
    ImGui::DragFloat3("Position",transform->localPosition.GetPtr(),1.f, -10000.f, 10000.f);
    ImGui::DragFloat3("Rotation",transform->localRotation.GetPtr(),1.f / Rad2Deg, -10000.f, 10000.f);
    ImGui::DragFloat3("Scale",transform->scale.GetPtr(),1.f, -10000.f, 10000.f);
    ImGui::PopID();
}

void Inspector::OnInput()
{
    if (ButtonCenteredOnLine("Add Component"))
    {
        ImGui::OpenPopup("Components");
    }

    const std::map<uint32_t, PC_CORE::ComponentRegister::RegisterComponentBackend>* componentMap = PC_CORE::ComponentRegister::componentRegisterMap;
    
    ImGui::SameLine();
    if (ImGui::BeginPopup("Components"))
    {
        ImGui::SeparatorText("Component");
        for (size_t i = 0; i < componentMap->size(); i++)
        {
            if (ImGui::Selectable(componentMap->at(i).name))
            {
                if (PC_CORE::World::world != nullptr)
                {
                    PC_CORE::World::world->scene.AddComponentInternal(static_cast<uint32_t>(i), m_Editor->selected);
                }
            } 
        }
            
        ImGui::EndPopup();
    }
}
