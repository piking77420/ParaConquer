#pragma once

#include "editor_command.hpp"
#include "ecs/ecs_header.h"

BEGIN_EDITOR_PCCORE
class EditorCommandCreateEntity : public EditorCommand
{
public:
    EditorCommandCreateEntity(Editor& _editor);

    ~EditorCommandCreateEntity() override;


private:
    PC_CORE::EntityId m_EntityID = PC_CORE::INVALID_ENTITY_ID;
};

END_EDITOR_PCCORE