#pragma once

#include <bitset>

#include "edit_world_window.hpp"
#include "ecs/ecs_header.h"

namespace PC_CORE
{
    class EntityManager;
}

BEGIN_EDITOR_PCCORE
    class Hierachy : public EditorWindow
{
public:

    Hierachy(Editor& _editor , const std::string& name);
    
    ~Hierachy() override = default;
    
    void Update() override;
private:
    void ShowGraph();

    PC_CORE::EntityManager* m_EntityManagerPtr = nullptr;

    std::bitset<PC_CORE::MAX_ENTITIES>* m_EnableEntitiesBitSetPtr = nullptr;
};

END_EDITOR_PCCORE