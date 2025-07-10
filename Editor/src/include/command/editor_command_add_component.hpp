#pragma once

#include "editor_command.hpp"
#include "ecs/ecs_header.h"
#include "ecs/component.h"

BEGIN_EDITOR_PCCORE


class EditorCommandAddComponent : public PC_EDITOR_CORE::EditorCommand
{
public:

    EditorCommandAddComponent(Editor& _editor, PC_CORE::EntityId _id, PC_CORE::TypeId _componentId);
    
    ~EditorCommandAddComponent() override;

private:
    PC_CORE::EntityId m_EntityId = PC_CORE::INVALID_ENTITY_ID;
    PC_CORE::TypeId m_ComponentId = PC_CORE::NullTypeId;
};


END_EDITOR_PCCORE
