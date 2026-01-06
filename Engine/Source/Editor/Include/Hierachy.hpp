#pragma once

#include <Bitset>

#include "EditWorldWindow.hpp"
#include "Ecs/EcsHeader.h"

namespace PC_CORE
{
    class EntityManager;
}

BEGIN_EDITOR_PCCORE
    class Hierachy : public EditorWindow
    {
    public:
        void Update() override;

        Hierachy(Editor& _editor, const std::string& name);

        ~Hierachy() override = default;

    private:
        void ShowGraph();

        PC_CORE::EntityManager* m_EntityManagerPtr = nullptr;

        std::bitset<PC_CORE::MAX_ENTITIES>* m_EnableEntitiesBitSetPtr = nullptr;
    };

END_EDITOR_PCCORE
