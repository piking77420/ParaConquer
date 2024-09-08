#pragma once

#include "vulkan_interface.hpp"

BEGIN_PCCORE

PC_CORE_API class RHI
{
public:
	static void Init(const RHICreateInfo& _RHICreateInfo);
   
	static void Destroy();


private:
	static inline RHIInterface* graphicInterface = nullptr;

	RHI() = delete;

	~RHI() = delete;
};


END_PCCORE