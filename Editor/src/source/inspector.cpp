#include "inspector.hpp"

#include "editor.hpp"
#include "imgui_helper.h"

using namespace PC_EDITOR_CORE;

void Inspector::Update()
{
    EditorWindow::Update();
    
    if(m_Editor->m_Selected == nullptr)
        return;

    Show();
    OnInput();
    
}

Inspector::Inspector(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
}

void Inspector::Show()
{
    PC_CORE::Scene* scene = &m_Editor->world.scene;
    auto vec = PC_CORE::Reflector::GetAllTypesFrom<PC_CORE::Component>();

    PC_CORE::Entity* selected = m_Editor->m_Selected;
    const char* selectedName = selected->name.c_str();
    ImGui::Text(selectedName);
    ImGui::PushID(selectedName); 

    for (size_t i = 0; i < vec.size(); i++)
    {
        const uint32_t currentKeyComponent = vec[i]->HashKey;

        if (!scene->HasComponent(selected->ecsId, currentKeyComponent))
            continue; 

        PC_CORE::Component* component = static_cast<PC_CORE::Component*>(scene->Get(selected->ecsId, currentKeyComponent));
        auto relfectedMember = PC_CORE::Reflector::GetType(currentKeyComponent);

        const char* componentName = vec[i]->name.c_str();
        // Component
        ImGui::Text(componentName);
        ImGui::Spacing();
    
        ImGui::PushID(i); 
        for (PC_CORE::Members& m : relfectedMember.membersKey)
        {
            ImGui::PushID((relfectedMember.name).c_str()); 
            ShowReflectedType(component, m); 
            ImGui::PopID(); 
            ImGui::Spacing();
        }
        ImGui::PopID(); // Pop the ID for the loop index
        DeleteButton(m_Editor->m_Selected, static_cast<uint32_t>(i));
    }

    ImGui::PopID(); 
 
}

void Inspector::OnInput()
{
    /*
    if (ButtonCenteredOnLine("Add Component"))
    {
        ImGui::OpenPopup("Components");
    }

    const std::map<uint32_t, PC_CORE::ComponentRegister::RegisterComponentBackend>* componentMap = &PC_CORE::ComponentRegister::componentRegisterMap;
    
    ImGui::SameLine();
    if (ImGui::BeginPopup("Components"))
    {
        
        ImGui::SeparatorText("Component");
        for (size_t i = 0; i < componentMap->size(); i++)
        {
            if (ImGui::Selectable(componentMap->at(static_cast<uint32_t>(i)).name.c_str()))
            {
                if (PC_CORE::World::world != nullptr)
                {
                    PC_CORE::World::world->scene.AddComponentInternal(static_cast<uint32_t>(i), m_Editor->selected);
                }
            } 
        }
            
        ImGui::EndPopup();
    }
    */
}
void Inspector::ShowReflectedType(void* begin, const PC_CORE::Members& _members)
{
    void* dataPosition = static_cast<char*>(begin) + _members.offset;
    const char* membersName = _members.membersName.c_str();
    
    switch (_members.dataNature)
    {
    case PC_CORE::DataNature::UNKNOW:
        break;
    case PC_CORE::DataNature::BOOL:
        ImGui::Checkbox(membersName, static_cast<bool*>(dataPosition));
        break;
    case PC_CORE::DataNature::INT:
        ImGui::DragInt(membersName, static_cast<int*>(dataPosition));
        break;
    case PC_CORE::DataNature::UINT:
        ImGui::DragInt(membersName, static_cast<int*>(dataPosition),0.1, 0);
        break;
    case PC_CORE::DataNature::FLOAT:
        ImGui::DragFloat(membersName, static_cast<float*>(dataPosition),0.1, 0);
        break;
    case PC_CORE::DataNature::DOUBLE:
        //ImGui::DragFloat(membersName, static_cast<double*>(dataPosition),1, 0);
        break;
    case PC_CORE::DataNature::VEC2:
        ImGui::DragFloat2(membersName, static_cast<float*>(dataPosition),0.1, 0);
        break;
    case PC_CORE::DataNature::VEC3:
        ImGui::DragFloat3(membersName, static_cast<float*>(dataPosition),0.1, 0);
        break;
    case PC_CORE::DataNature::VEC4:
    case PC_CORE::DataNature::QUAT:
        ImGui::DragFloat4(membersName, static_cast<float*>(dataPosition),0.1, 0);
        break;
    case PC_CORE::DataNature::COMPOSITE:
        for (const PC_CORE::Members& m : PC_CORE::Reflector::GetType(_members.key).membersKey)
        {
            void* DatadataPosition = static_cast<char*>(dataPosition) + m.offset;
            ImGui::PushID((m.membersName).c_str());
            ShowReflectedType(DatadataPosition, m);
            ImGui::PopID();
            ImGui::Spacing();
        }
        break;
    case PC_CORE::DataNature::COUT:
        break;
    }

}

    
void Inspector::DeleteButton(PC_CORE::Entity* _entity, uint32_t _componentId)
{
    if (ImGui::SmallButton("Delete Component"))
    {
        m_Editor->world.scene.RemoveComponent(_entity->ecsId, _componentId);
    }
}
    
