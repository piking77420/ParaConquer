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
    const std::map<uint32_t, PC_CORE::ComponentRegister::RegisterComponentBackend>* componentMap = &PC_CORE::ComponentRegister::componentRegisterMap;


    auto entityInternal = PC_CORE::World::world->scene.GetEntityInternal(m_Editor->selected);
    ImGui::Text(entityInternal->name.c_str());
    ImGui::PushID(entityInternal->name.c_str());
    
    for (size_t i = 0; i < componentMap->size(); i++)
    {
        std::vector<uint8_t>* componentData = nullptr;
        PC_CORE::World::world->scene.GetComponentDataRaw(static_cast<uint32_t>(i), &componentData);

        const uint32_t ComponnentIndex = entityInternal->componentIdIndexInDataArray[i];
        if (ComponnentIndex == NULL_COMPONENT)
            continue;

        const PC_CORE::ComponentRegister::RegisterComponentBackend& componentBackend = componentMap->at(i);
        const size_t componentIndextoUint = ComponnentIndex;
        void* currentComponent = &componentData->at(componentIndextoUint);
        
        if (componentMap->at(static_cast<uint32_t>(i)).reflecteds.empty())
            continue;
        ImGui::PushID(i);

        ImGui::Spacing();
        ImGui::Text(componentBackend.name.c_str());
        DeleteButton(m_Editor->selected, static_cast<uint32_t>(i));
        for (const PC_CORE::ReflectionType& refl : componentMap->at(static_cast<uint32_t>(i)).reflecteds)
        {
            ImGui::PushID("Refleted Type");
            ShowReflectedType(currentComponent, refl);
            ImGui::PopID();
        }
        ImGui::PopID();

    }
    ImGui::PopID();
}

void Inspector::OnInput()
{
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
}

void Inspector::ShowReflectedType(void* begin, const PC_CORE::ReflectionType& reflection)
{
    void* dataPosition = static_cast<char*>(begin) + reflection.offset;

    switch (reflection.datatype)
    {
    case PC_CORE::DataType::UNKNOW:
        break;
    case PC_CORE::DataType::INT:
        ImGui::DragInt(reflection.name, static_cast<int*>(dataPosition));
        break;
    case PC_CORE::DataType::UINT:
        ImGui::DragInt(reflection.name, static_cast<int*>(dataPosition),0.1, 0);
        break;
    case PC_CORE::DataType::FLOAT:
        ImGui::DragFloat(reflection.name, static_cast<float*>(dataPosition),0.1, 0);
        break;
    case PC_CORE::DataType::DOUBLE:
        //ImGui::DragFloat(reflection.name, static_cast<double*>(dataPosition),1, 0);
        break;
    case PC_CORE::DataType::VEC2:
        ImGui::DragFloat2(reflection.name, static_cast<float*>(dataPosition),0.1, 0);
        break;
    case PC_CORE::DataType::VEC3:
        ImGui::DragFloat3(reflection.name, static_cast<float*>(dataPosition),0.1, 0);
        break;
    case PC_CORE::DataType::VEC4:
        ImGui::DragFloat4(reflection.name, static_cast<float*>(dataPosition),0.1, 0);
        break;
    case PC_CORE::DataType::COUT:
        break;
    }
}

void Inspector::DeleteButton(Entity _entity, uint32_t _componentId)
{
    if (ImGui::SmallButton("Delete Component"))
    {
        m_Editor->world.scene.RemoveComponentInternal(_componentId, _entity);
    }
}
