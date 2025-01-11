#pragma once

#include "core_header.hpp"
#include "rendering/rendering_typedef.h"
//#include "rendering/render_harware_interface/command_buffer.h"

BEGIN_PCCORE
	class  IMGUIContext
{
public:
	PC_CORE_API static void Init(void* _glfwWindowPtr, PC_CORE::GraphicAPI _graphicApi);

	PC_CORE_API static void Destroy();

	PC_CORE_API static void NewFrame();

	//PC_CORE_API static void Render(PC_CORE::CommandBuffer _commandBuffer);
	
	PC_CORE_API IMGUIContext() = delete;

	PC_CORE_API ~IMGUIContext() = delete;

private:
	//static inline PC_CORE::DescriptorPoolHandle m_DescriptorPoolHandle = NULL_HANDLE;
};

END_PCCORE