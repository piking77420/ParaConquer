#pragma once

#include "rhi_typedef.hpp"

BEGIN_PCCORE

PC_CORE_API class RHIInterface
{
public:

	RHIInterface(const RHICreateInfo& _rHICreateInfo)
	{

	}

	virtual ~RHIInterface() = default;

protected:
};

END_PCCORE