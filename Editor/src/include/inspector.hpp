#pragma once

#include "edit_world_window.hpp"
#include "reflection/reflection_typedef.hpp"
#include "world/scene.hpp"
#include "world/transform.hpp"

BEGIN_EDITOR_PCCORE
    class Inspector : public EditorWindow
{
public:
    ~Inspector() override ;
    
    void Update() override;

    Inspector(Editor& _editor, const std::string& _name);

private:
    void Show();

    void OnInput();

    void ShowReflectedType(void* begin, const PC_CORE::Members& _members);

    void DeleteButton(PC_CORE::Entity* _entity, uint32_t _componentId);

    void PrintArray(void* begin, const PC_CORE::Members& _members);

    void PrintTransform(PC_CORE::Transform* _transform);

    std::vector<const PC_CORE::ReflectedType*> m_ReflectedTypes;

    const PC_CORE::ReflectedType* transfromTypeRef;
};

END_EDITOR_PCCORE