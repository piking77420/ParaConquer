#pragma once

#include "core_header.hpp"
#include "rendering/renderer.hpp"
#include "window.hpp"
#include "rendering/camera.hpp"
#include "world/world.hpp"
#include "input/low_pass_filter.hpp"
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
		
	virtual void Init();

	virtual void Destroy();
	
	App() = default;

	~App() = default;

	virtual void Run();
	
	void WorldLoop();

	void HandleResize();
	
	static inline App* instance = nullptr;


protected:
};

END_PCCORE