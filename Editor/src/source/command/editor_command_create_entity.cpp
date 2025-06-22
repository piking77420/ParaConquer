#include "command/editor_command_create_entity.hpp"

#include "editor.hpp"
#include "world/world.hpp"

PC_EDITOR_CORE::EditorCommandCreateEntity::EditorCommandCreateEntity(Editor& _editor) : EditorCommand(_editor)
{
    m_EntityID = m_Editor.gameApp.world.level.CreateEntity();
}

PC_EDITOR_CORE::EditorCommandCreateEntity::~EditorCommandCreateEntity()
{
    if (m_EntityID == PC_CORE::INVALID_ENTITY_ID)
        return;
    m_Editor.gameApp.world.level.RemoveEntity(m_EntityID);
}
