#pragma once

#include "edit_world_window.hpp"
#include "reflection/reflection_typedef.hpp"
#include "world/world_header_typedef.hpp"

BEGIN_EDITOR_PCCORE
    class Inspector : public EditorWindow
{
public:
    ~Inspector() override = default;
    
    void Update() override;

    Inspector(Editor& _editor, const std::string& _name);

private:
    void Show();

    void OnInput();

    void ShowReflectedType(void* begin,const PC_CORE::ReflectionType& reflection);

    void DeleteButton(Entity _entity, uint32_t _componentId);
};

END_EDITOR_PCCORE