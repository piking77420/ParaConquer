#include "world/world.hpp"

using namespace PC_CORE;

void World::Run()
{
    if (!begin)
        return;
    
    if (run == false)
    {
        scene.Begin();
        run = true;
    }

    if (run)
    {
        scene.Update();
    }
    
}
