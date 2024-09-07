#include "inspector.hpp"

#include "editor.hpp"
#include "imgui_helper.h"

using namespace PC_EDITOR_CORE;

void Inspector::Update()
{
    EditorWindow::Update();
    
    if(m_Editor->m_Selected == nullptr)
        return;

    m_ReflectedTypes = PC_CORE::Reflector::GetAllTypesFrom<PC_CORE::Component>();
    Show();
    OnInput();
    
}

Inspector::Inspector(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
}

void Inspector::Show()
{
    PC_CORE::Scene* scene = &m_Editor->world.scene;

    PC_CORE::Entity* selected = m_Editor->m_Selected;
    const char* selectedName = selected->name.c_str();
    ImGui::Text(selectedName);
    ImGui::PushID(selectedName); 

    for (size_t i = 0; i < m_ReflectedTypes.size(); i++)
    {
        const uint32_t currentKeyComponent = m_ReflectedTypes[i]->HashKey;

        if (!scene->HasComponent(selected->ecsId, currentKeyComponent))
            continue; 

        PC_CORE::Component* component = static_cast<PC_CORE::Component*>(scene->Get(selected->ecsId, currentKeyComponent));
        auto relfectedMember = PC_CORE::Reflector::GetType(currentKeyComponent);

        const char* componentName = m_ReflectedTypes[i]->name.c_str();
        // Component
        ImGui::Text(componentName);
        ImGui::Spacing();
    
        ImGui::PushID(i); 
        for (PC_CORE::Members& m : relfectedMember.members)
        {
            ImGui::PushID((relfectedMember.name).c_str()); 
            ShowReflectedType(component, m); 
            ImGui::PopID(); 
            ImGui::Spacing();
        }
        DeleteButton(m_Editor->m_Selected, m_ReflectedTypes[i]->HashKey);
        ImGui::PopID(); // Pop the ID for the loop index
    }

    ImGui::PopID(); 
 
}

void Inspector::OnInput()
{
    if (ButtonCenteredOnLine("Add Component"))
    {
        ImGui::OpenPopup("Components");
    }

    
    ImGui::SameLine();
    if (ImGui::BeginPopup("Components"))
    {
        
        ImGui::SeparatorText("Component");
        for (auto& m_ReflectedType : m_ReflectedTypes)
        {
            if (ImGui::Selectable(m_ReflectedType->name.c_str()))
            {
                if (PC_CORE::World::world != nullptr)
                {
                    PC_CORE::World::world->scene.AddComponent(m_Editor->m_Selected->ecsId, m_ReflectedType->HashKey);
                }
            } 
        }
            
        ImGui::EndPopup();
    }
    
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
        if (_members.enumFlag & PC_CORE::MemberEnumFlag::COLOR)
        {
            ImGui::ColorPicker3(membersName, static_cast<float*>(dataPosition),ImGuiColorEditFlags_PickerHueWheel);
        }
        else
        {
            ImGui::DragFloat3(membersName, static_cast<float*>(dataPosition),0.1, 0);
        }
        break;
    case PC_CORE::DataNature::VEC4:
    case PC_CORE::DataNature::QUAT:
        if (_members.enumFlag & PC_CORE::MemberEnumFlag::COLOR)
        {
            ImGui::ColorPicker4(membersName, static_cast<float*>(dataPosition),ImGuiColorEditFlags_PickerHueWheel);
        }
        else
        {
            ImGui::DragFloat4(membersName, static_cast<float*>(dataPosition),0.1, 0);
        }
        break;
    case PC_CORE::DataNature::COMPOSITE:
        for (const PC_CORE::Members& m : PC_CORE::Reflector::GetType(_members.typeKey).members)
        {
            void* DatadataPosition = static_cast<char*>(dataPosition) + m.offset;
            ImGui::PushID((m.membersName).c_str());
            ShowReflectedType(DatadataPosition, m);
            ImGui::PopID();
            ImGui::Spacing();
        }
        break;
    case PC_CORE::DataNature::COUNT:
        break;
    default: ;
    }

}

    
void Inspector::DeleteButton(PC_CORE::Entity* _entity, uint32_t _componentId)
{
    if (ImGui::SmallButton("Delete Component"))
    {
        m_Editor->world.scene.RemoveComponent(_entity->ecsId, _componentId);
    }
}
    
