#pragma once

#include "core_header.hpp"
#include "rendering/renderer.hpp"
#include "window.hpp"
#include "rendering/camera.hpp"
#include "world/world.hpp"
#include "scripting/scripting_lua.hpp"
#include <core/physics_engine.h>

#include "ecs/ecs_context.h"


BEGIN_PCCORE
	class App
{
public:
	static constexpr const char* appName = "ParaConquer";
	
	Window windowHandle;

	Renderer renderer;

	ScriptingLua scriptingLua;

	PhysicsWrapper physicsWrapper;
	
	World world;
		
	PC_CORE_API virtual void Init();

	PC_CORE_API virtual void Destroy();
	
	PC_CORE_API App() = default;

	PC_CORE_API ~App() = default;

	PC_CORE_API virtual void Run();
	
	PC_CORE_API void WorldLoop();

	PC_CORE_API void HandleResize();
	
	PC_CORE_API static inline App* instance = nullptr;


protected:
};

END_PCCORE