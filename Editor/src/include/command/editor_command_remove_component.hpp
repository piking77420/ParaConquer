#pragma once

#include "editor_command_create_entity.hpp"

BEGIN_EDITOR_PCCORE
    class EditorCommandRemoveComponent : public EditorCommand
{
public:
    explicit EditorCommandRemoveComponent(Editor& _editor, PC_CORE::EntityId _id,
                                                                     PC_CORE::TypeId _componentId);
    
    ~EditorCommandRemoveComponent() override;

protected:
    PC_CORE::EntityId m_EntityId = PC_CORE::INVALID_ENTITY_ID;
    PC_CORE::TypeId m_ComponentId = PC_CORE::NullTypeId;
};

END_EDITOR_PCCORE