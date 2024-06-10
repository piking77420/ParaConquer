#pragma once

#include "core_header.hpp"
#include "rendering/renderer.hpp"
#include "window.hpp"
#include "rendering/camera.hpp"
#include "rendering/vulkan/vulkan_imgui.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "world/world.hpp"
#include "input/low_pass_filter.hpp"
#include "rendering/light.hpp"
#include "world/transform.hpp"

BEGIN_PCCORE
class App
{
public:
	static constexpr const char* appName = "ParaConquer";
	
	Window windowHandle;

	Renderer renderer;

	VulkanImgui vulkanImgui; 

	World world;

	virtual void Init();

	virtual void Destroy();
	
	App() = default;

	~App() = default;

	virtual void Run();

	void HandleResize();
protected:
	void MoveObject();

	void InitScene();

	std::vector<Transform*> transforms;

	DirLight* dirLight = nullptr;
};

END_PCCORE