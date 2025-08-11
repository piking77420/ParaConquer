#pragma once

#include "EditWorldWindow.hpp"
#include "Ecs/Archetype.h"
#include "Ecs/ComponentsManager.hpp"
#include "Physics/SphereCollider.hpp"
#include "Reflection/ReflectionTypedef.hpp"
#include "World/Scene.hpp"
#include "World/Transform.hpp"



BEGIN_EDITOR_PCCORE
    class Inspector : public EditorWindow
{
public:
    ~Inspector() override ;
    
    void Update() override;

    Inspector(Editor& _editor, const std::string& _name);

private:

    void ShowEntity(PC_CORE::EntityId _id);

    void Show();

    void ShowResource(PC_CORE::Resource* _resource);
    
    void ShowReflectType(uint8_t* _typePtr, const PC_CORE::ReflectedType& _reflectedType);

    void ShowMember(uint8_t* _memberPtr, const PC_CORE::Members& _members);

    bool IsShowable(PC_CORE::TypeId type_id);
    
    std::vector<const PC_CORE::ReflectedType*> m_ComponentReflected;
    
    std::vector<const PC_CORE::ReflectedType*> m_ResourceReflected;

    std::vector<const PC_CORE::ReflectedType*> m_SpecialType;

    void HandleShowAble(uint8_t* ptr, const PC_CORE::ReflectedType& type, const PC_CORE::Members& _typeAsMember);

    void HandlePtr(uint8_t* ptr, const PC_CORE::ReflectedType& type, const PC_CORE::Members& _typeAsMemberr);

    template <typename T>
    T* GetPtrToData(uint8_t* ptr);

    PC_CORE::EntityManager* entityManagerPtr = nullptr;

    PC_CORE::ComponentManager* componentManagerPtr = nullptr;

    PC_CORE::ComponentArrayMap* componentArrayMapPtr = nullptr;

    std::unordered_map<PC_CORE::ComponentTypeBit, PC_CORE::TypeId >* componentTypeBitToTypeId = nullptr;
    
};

    template <typename T>
    T* Inspector::GetPtrToData(uint8_t* ptr)
    {
        if constexpr (std::is_pointer_v<T>)
        {
            return reinterpret_cast<T*>(ptr);
        }
        else
        {
            return *reinterpret_cast<T*>(ptr);
        }
        
    }


END_EDITOR_PCCORE
