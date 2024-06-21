#include "inspector.hpp"

#include "editor.hpp"

using namespace PC_EDITOR_CORE;

void Inspector::Update()
{
    EditorWindow::Update();
    Show();
}

Inspector::Inspector(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
}

void Inspector::Show()
{
    if(m_Editor->selected == NULL_ENTITY)
        return;

    PC_CORE::Transform* transform = PC_CORE::World::world->scene.GetComponent<PC_CORE::Transform>(m_Editor->selected);
    if (transform == nullptr)
        return;
    
    ImGui::PushID(transform->componentHolder.entityID);
    ImGui::Text("entity %d",transform->componentHolder.entityID);
    ImGui::DragFloat3("Position",transform->localPosition.GetPtr(),1.f, -10000.f, 10000.f);
    ImGui::DragFloat3("Rotation",transform->localRotation.GetPtr(),1.f / Rad2Deg, -10000.f, 10000.f);
    ImGui::DragFloat3("Scale",transform->scale.GetPtr(),1.f, -10000.f, 10000.f);
    ImGui::PopID();
}
