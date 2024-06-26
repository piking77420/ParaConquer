#pragma once

#include "edit_world_window.hpp"
#include "reflection/reflection_typedef.hpp"

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
};

END_EDITOR_PCCORE