#include "world/world.hpp"

using namespace PC_CORE;


World::World()
{
    if (m_World != nullptr)
    {
        PC_LOGERROR("World already initialized");
        throw std::runtime_error("World already initialized");
    }
    
    m_World = this;
}

void World::Begin()
{
    //scene.Begin();
}

void World::Update()
{
    //scene.Update();
}

void World::Destroy()
{
    m_World = nullptr;
}

