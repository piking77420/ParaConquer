#pragma once

#include "core_header.hpp"
#include "world/world.hpp"
#include "scripting/scripting_lua.hpp"
#include <core/physics_engine.h>

#include "io/window.hpp"
#include "ecs/ecs_context.h"
#include <io/core_io.hpp>

#include "low_renderer/rhi.hpp"
#include "rendering/renderer.hpp"


BEGIN_PCCORE
	class App
{
public:
	static constexpr const char* appName = "ParaConquer";

	CoreIo coreIo;

	Window window;

	Rhi rhi;

	Renderer renderer;
	
	ScriptingLua scriptingLua;

	PhysicsWrapper physicsWrapper;
	
	World world;
		
	PC_CORE_API virtual void Init();

	PC_CORE_API virtual void Destroy();
	
	PC_CORE_API App();

	PC_CORE_API ~App() = default;

	PC_CORE_API virtual void Run();
	
	PC_CORE_API void WorldLoop();
	
	PC_CORE_API static inline App* instance = nullptr;


protected:
};

END_PCCORE