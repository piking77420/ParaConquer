#include "world/world.hpp"

using namespace PC_CORE;



void World::Begin()
{
    scene.Begin();
}

void World::Update()
{
    scene.Update();
}

void World::LoadSkyBox()
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

