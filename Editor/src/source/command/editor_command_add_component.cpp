#include "command/editor_command_add_component.hpp"

#include "editor.hpp"

PC_EDITOR_CORE::EditorCommandAddComponent::EditorCommandAddComponent(Editor& _editor, const PC_CORE::EntityId _id,
                                                                     PC_CORE::TypeId _componentId) : EditorCommand(_editor), m_EntityId(_id), m_ComponentId(_componentId)
{
    if (m_EntityId != PC_CORE::INVALID_ENTITY_ID && m_ComponentId != PC_CORE::NullTypeId)
        m_Editor.gameApp.world.level.AddComponent(m_EntityId, m_ComponentId);
}

PC_EDITOR_CORE::EditorCommandAddComponent::~EditorCommandAddComponent()
{
    if (m_EntityId != PC_CORE::INVALID_ENTITY_ID && m_ComponentId != PC_CORE::NullTypeId)
        m_Editor.gameApp.world.level.RemoveComponent(m_EntityId, m_ComponentId);
}
