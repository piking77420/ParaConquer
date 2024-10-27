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
    
    
    std::string* string = &selected->name;
    ImGui::PushID("EntityNameInput");
    ImGui::InputText("##EntityName", string->data(), string->size());
    ImGui::PopID();

    for (size_t i = 0; i < m_ReflectedTypes.size(); i++)
    {
        const uint32_t currentKeyComponent = m_ReflectedTypes[i]->HashKey;

        if (!scene->HasComponent(selected->ecsId, currentKeyComponent))
            continue;

        PC_CORE::Component* component = static_cast<PC_CORE::Component*>(scene->GetComponent(selected->ecsId, currentKeyComponent));
        auto reflectedMember = PC_CORE::Reflector::GetType(currentKeyComponent);

        const char* componentName = m_ReflectedTypes[i]->name.c_str();
        ImGui::Text(componentName);
        ImGui::Spacing();

        ImGui::PushID(static_cast<int>(currentKeyComponent));

        for (size_t j = 0; j < reflectedMember.members.size(); j++)
        {
            PC_CORE::Members& m = reflectedMember.members[j];
            ImGui::PushID(static_cast<int>(currentKeyComponent + j));
            ShowReflectedType(component, m);
            ImGui::PopID();
            ImGui::Spacing();
        }

        DeleteButton(m_Editor->m_Selected, m_ReflectedTypes[i]->HashKey);

        ImGui::PopID(); 
    }
    
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
                if (PC_CORE::World::currentWorld != nullptr)
                {
                    PC_CORE::World::currentWorld->scene.AddComponent(m_Editor->m_Selected->ecsId, m_ReflectedType->HashKey);
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
    const PC_CORE::ReflectedType& type = PC_CORE::Reflector::GetType(_members.typeKey);

    if (type.typeInfo.typeInfoFlags & PC_CORE::TypeFlag::COMPOSITE)
    {
        for (const PC_CORE::Members& m : type.members)
        {
            void* dataPosMember = static_cast<uint8_t*>(dataPosition) + m.offset;
            ImGui::PushID((m.membersName).c_str());
            ShowReflectedType(dataPosMember, m);
            ImGui::PopID();
            ImGui::Spacing();
        }
    }
    else if (type.typeInfo.typeInfoFlags & PC_CORE::TypeFlag::ARRAY)
    {
       
    }
    else
    {
        switch (type.typeInfo.dataNature)
        {
        case PC_CORE::DataNature::UNKNOWN:
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
        case PC_CORE::DataNature::STRING :
            {
             
                std::string* string = static_cast<std::string*>(dataPosition);
                ImGui::InputText(membersName, string->data(), string->size());
                break;   
            }
        case PC_CORE::DataNature::COUNT:
            {
                
            }
            break;
        default: ;
        }
        
    }
    
}

    
void Inspector::DeleteButton(PC_CORE::Entity* _entity, uint32_t _componentId)
{
    
    if (ImGui::SmallButton("Delete Component"))
    {
        m_Editor->world.scene.RemoveComponent(_entity->ecsId, _componentId);
    }
}

void Inspector::PrintArray(void* begin, const PC_CORE::Members& _members)
{
    
}
    
