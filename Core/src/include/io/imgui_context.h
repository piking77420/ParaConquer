#pragma once

#include "core_header.hpp"
#include "low_renderer/rhi.hpp"

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