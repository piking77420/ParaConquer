#include "world/world.hpp"

using namespace PC_CORE;

World::World()
{
    std::array<std::string, 6> skyboxImage =
    {
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg"
    };

    skybox.Load(skyboxImage);
}

void World::Run()
{
    sceneGraph.UpdateTransforms(&scene);
    
    if (begin && run == false)
    {
        scene.Begin();
        run = true;
    }
    
    if (run)
    {
        scene.Update();
    }
    
    sceneGraph.UpdateMatrix(&scene);
}
