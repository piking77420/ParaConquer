#pragma once

#include "edit_world_window.hpp"
#include "ecs/archetype.h"
#include "physics/sphere_collider.hpp"
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
    enum class ShowableReflect
    {
        // ENUMATE ALL SHOWALBEL TYPE 
    };

    
    void Show();

    void OnInput();

    void ShowReflectedType_Unused(void* begin, const PC_CORE::Members& _members);

    bool HandleSpecialType(const PC_CORE::ReflectedType& type, uintmax_t memberFlag ,const char* _memberName,  void* begin);
    
    void PrintArray(uint8_t* begin, const PC_CORE::Members& _members);

    void ShowReflectType(uint8_t* _typePtr, const PC_CORE::ReflectedType& _reflectedType);

    void ShowMember(uint8_t* _memberPtr, const PC_CORE::Members& _members);

    bool IsShowable(PC_CORE::TypeId type_id);
    
    std::vector<const PC_CORE::ReflectedType*> m_ReflectedTypes;

    std::vector<const PC_CORE::ReflectedType*> m_SpecialType;

    void HandleShowAble(uint8_t* ptr, const PC_CORE::ReflectedType& type, const PC_CORE::Members& _typeAsMember);

    void HandlePtr(uint8_t* ptr, const PC_CORE::ReflectedType& type, const PC_CORE::Members& _typeAsMemberr);

};

END_EDITOR_PCCORE