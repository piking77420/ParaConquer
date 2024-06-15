#pragma once


#include "scene.hpp"
#include "scene_graph.hpp"
#include "skybox.hpp"
#include "world_header_typedef.hpp"

BEGIN_PCCORE

class World
{
public:
    static inline World* world = nullptr;

    Scene scene;

    SceneGraph sceneGraph;

    Skybox skybox;

    bool begin = false;

    bool run = false;

    World();
    
    ~World() = default;
    
    void Run();

private:
};

END_PCCORE