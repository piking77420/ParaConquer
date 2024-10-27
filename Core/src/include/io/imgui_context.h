#pragma once

#include "core_header.hpp"

BEGIN_PCCORE

class  IMGUIContext
{
public:
	PC_CORE_API static void Init(void* _glfwWindowPtr);

	PC_CORE_API static void Destroy();

	PC_CORE_API static void NewFrame();

	PC_CORE_API static void Render();


	PC_CORE_API IMGUIContext() = delete;

	PC_CORE_API ~IMGUIContext() = delete;
};

END_PCCORE