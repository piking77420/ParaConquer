#pragma once


#include "skybox.hpp"
#include "physics/physics_wrapper.hpp"

BEGIN_PCCORE
class World
{
public:
    EntityManager entityManager;

    Skybox skybox;
    
    bool begin = false;

    bool run = false;

    PC_CORE_API World();
    
    PC_CORE_API ~World() = default;

    PC_CORE_API void Begin();

    PC_CORE_API void Update();

    PC_CORE_API void LoadSkyBox();

    PC_CORE_API static World* GetWorld();
    
private:
    static inline World* m_World = nullptr;
};

END_PCCORE