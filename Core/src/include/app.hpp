#pragma once

#include "core_header.hpp"
#include "rendering/renderer.hpp"
#include "window.hpp"
#include "rendering/camera.hpp"
#include "rendering/vulkan/vulkan_imgui.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "world/world.hpp"
#include "input/low_pass_filter.hpp"
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

	Camera camera;
	
	float cameraSpeed = 5.f;
	float pitch = 0.f;
	float yaw = 0.f;

	LowPassFilter<Vector2f, 12> deltass;
	
	App();

	~App();

	void Run();

	void HandleResize();
private:
	void MoveCam();

	void MoveObject();

	std::vector<Transform*> transforms;
};

END_PCCORE