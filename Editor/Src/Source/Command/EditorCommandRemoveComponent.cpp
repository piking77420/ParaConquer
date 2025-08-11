#include "Command/EditorCommandRemoveComponent.hpp"

#include "Editor.hpp"



PC_EDITOR_CORE::EditorCommandRemoveComponent::EditorCommandRemoveComponent(Editor& _editor, PC_CORE::EntityId _id,
    PC_CORE::TypeId _componentId) : EditorCommand(_editor), m_EntityId(_id), m_ComponentId(_componentId)
{
    if (m_EntityId != PC_CORE::INVALID_ENTITY_ID && m_ComponentId != PC_CORE::NullTypeId)
    {
        const auto& t = PC_CORE::Reflector::GetType(m_ComponentId);
        uint8_t* ptr = &m_Editor.gameApp.world.level.GetComponent(m_EntityId, m_ComponentId);

        m_ComponentData = std::make_unique<uint8_t[]>(t.size);
        std::memcpy(m_ComponentData.get(), ptr, t.size);

        m_Editor.gameApp.world.level.RemoveComponent(m_EntityId, m_ComponentId);
    }
}

PC_EDITOR_CORE::EditorCommandRemoveComponent::~EditorCommandRemoveComponent()
{
    if (m_EntityId != PC_CORE::INVALID_ENTITY_ID && m_ComponentId != PC_CORE::NullTypeId)
    {
        m_Editor.gameApp.world.level.AddComponent(m_EntityId, m_ComponentId);

        const auto& t = PC_CORE::Reflector::GetType(m_ComponentId);
        uint8_t* ptr = &m_Editor.gameApp.world.level.GetComponent(m_EntityId, m_ComponentId);
        std::memcpy(ptr, m_ComponentData.get(), t.size);

    }
}
