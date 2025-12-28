#pragma once

#include "Level.hpp"
#include "Ecs/EntityManager.h"
#include "Ecs/ComponentsManager.hpp"
#include "Ecs/SystemManager.hpp"
#include "Rendering/RenderingTypedef.h"

BEGIN_PCCORE
    class World
    {
    public:
        PC_CORE_API FORCEINLINE static World* GetWorld()
        {
            return m_World;
        }

        bool begin = false;

        bool run = false;

        Level level;

        Rendering::RenderingWorldData RenderingWorldData; // TODO DO HANDLE RENDER THREAD

        PC_CORE_API void Begin();

        PC_CORE_API void Update(double _tick);

        PC_CORE_API void RenderingTick(double _tick);

        PC_CORE_API void LoadLevel(const Level& _level);

        PC_CORE_API World();

        PC_CORE_API ~World()
        {
            m_World = nullptr;
        };

    private:
        PC_CORE_API static inline World* m_World = nullptr;


        //void GetStaticMesh(RenderingWorldData& _renderData, EntityId _entityId);

        //void GetStaticLightData(RenderingWorldData& _renderData, EntityId _entityId);

        REFLECT(World)
        REFLECT_MEMBER(World, level)
    };


END_PCCORE
