#include "world/world.hpp"
#include <debug_helper/debug_draw_context.hpp>

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
    PERF_REGION_SCOPED;

    //DebugDrawContext::DrawSphere(Tbx::Vector3d{ 0,20,0 }, 10, Tbx::Vector3f(0.5, 1, 0));


    if (run)
    {
        //TO DO CALL SYS UpDATE

        
    }
}


void World::LoadLevel(const Level& _level)
{
    level = _level;
}

