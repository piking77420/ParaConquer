#pragma once


#include "scene.hpp"
#include "scene_graph.hpp"
#include "skybox.hpp"
#include "physics/physics_wrapper.hpp"

BEGIN_PCCORE
    class World
{
public:
    static inline World* world = nullptr;

	EcsContext ecsContext;

    Scene scene;

    SceneGraph sceneGraph;

    Skybox skybox;
    
    bool begin = false;

    bool run = false;

    PC_CORE_API World() = default;
    
    PC_CORE_API ~World() = default;

    PC_CORE_API void Begin();

    PC_CORE_API void Update();

    PC_CORE_API void LoadSkyBox();

private:
};

END_PCCORE