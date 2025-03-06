#pragma once

#include "app.hpp"
#include "rendering/camera.hpp"

class Runtime
{
public:
	Runtime(bool* m_AppSouldClose);
	
	~Runtime();

	void Run();

private:
	PC_CORE::App m_App;

	bool* m_AppSouldClose;

	PC_CORE::Camera camera;
};

