#pragma once


#include "scene.hpp"
#include "scene_graph.hpp"
#include "world_header_typedef.hpp"

BEGIN_PCCORE

class World
{
public:
    static inline World* world = nullptr;

    Scene scene;

    SceneGraph sceneGraph;

    bool begin = false;

    bool run = false;

    World() = default;
    
    ~World() = default;
    
    void Run();

private:
};

END_PCCORE