#include "Command/EditorCommandCreateEntity.hpp"

#include "Editor.hpp"
#include "World/World.hpp"

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
