#pragma once

#include "EditorCommandCreateEntity.hpp"

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

    std::unique_ptr<uint8_t[]> m_ComponentData;
};

END_EDITOR_PCCORE