#include "world/world.hpp"
#include <DebugHelper/DebugDrawContext.hpp>

#include "Rendering/Light.hpp"
#include "World/Transform.hpp"

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
        level.Begin();
        run = true;
    }
}

void World::Update(double _tick)
{
    PERF_REGION_SCOPED;

    if (run)
    {
        level.Update(_tick);
    }
}

void World::RenderingTick(double _tick)
{
    level.RenderingTick(_tick);
}


void World::LoadLevel(const Level& _level)
{
    //level = _level;
    assert(false);
}
