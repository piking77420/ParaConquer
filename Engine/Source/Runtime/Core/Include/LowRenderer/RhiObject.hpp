#pragma once

#include <string>
#include <string_view>

#include "CoreHeader.hpp"
#include "RhiTypedef.h"


BEGIN_PCCORE

class Rhi;

class RhiObject
{
public:

	PC_CORE_API virtual ~RhiObject();

	PC_CORE_API explicit RhiObject(Rhi& rhi, const std::string& _name);

    PC_CORE_API explicit RhiObject(Rhi& rhi, std::string&& _name);

	PC_CORE_API virtual const void* GetFrameNativeHandle(size_t _frameIndex) const = 0;

	PC_CORE_API virtual void* GetFrameNativeHandle(size_t _frameIndex) = 0;

	PC_CORE_API virtual bool Build() = 0;

protected:
	inline std::string_view GetName() const noexcept
	{
		return std::string_view(m_Name);
	}

	Rhi& m_Rhi;

private:
	std::string m_Name;

};

END_PCCORE

