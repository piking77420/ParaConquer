#pragma once

#include "core_header.hpp"
#include "rendering/renderer.hpp"
#include "window.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "world/world.hpp"

BEGIN_PCCORE
	class App
{
public:
	static constexpr const char* appName = "ParaConquer";
	
	Window windowHandle;

	Renderer renderer;

	World world;
	
	App();

	~App();

	void Run();
};

END_PCCORE