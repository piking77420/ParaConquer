#pragma once

#include "EditorCommand.hpp"
#include "Ecs/EcsHeader.h"

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
