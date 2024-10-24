﻿#pragma once


#include "scene.hpp"
#include "scene_graph.hpp"
#include "skybox.hpp"
#include "world_header_typedef.hpp"
#include "physics/physics_wrapper.hpp"

BEGIN_PCCORE
    class World
{
public:
    static inline World* world = nullptr;

    Scene scene;

    SceneGraph sceneGraph;

    Skybox skybox;

    PhysicsWrapper physicsWrapper;

    bool begin = false;

    bool run = false;

    World() = default;
    
    ~World() = default;

    void Begin();

    void Update();

    void LoadSkyBox();

private:
};

END_PCCORE