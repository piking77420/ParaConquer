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
    if (begin)
    {
        PERF_REGION_SCOPED;
        //TO DO CALL SYS BEGIN
        begin = false;
        run = true;
    }
}

void World::Update()
{
    if (run)
    {
        //TO DO CALL SYS UpDATE
        Update();
    }
}

void World::Destroy()
{
    m_World = nullptr;
}

