#pragma once


#include "level.hpp"
#include "skybox.hpp"
#include "physics/physics_wrapper.hpp";
#include "ecs/entity_manager.h"
#include "ecs/components_manager.hpp"
#include "ecs/system_manager.hpp"

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
    
    PC_CORE_API void Begin();

    PC_CORE_API void Update();
    
    PC_CORE_API void LoadLevel(const Level& _level);
    
    PC_CORE_API World();
    
    PC_CORE_API ~World()
    {
        m_World = nullptr;
    };

private:
    PC_CORE_API static inline World* m_World = nullptr;
    
    REFLECT(World)
    REFLECT_MEMBER(World, level)
};




END_PCCORE