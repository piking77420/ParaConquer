#include "Rendering/Buffer/IndexBuffer.hpp"

#include "Utils/RhiToVulkan.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/RhiBuffer.h"

namespace PC_CORE
{
	IndexBuffer::IndexBuffer(Rhi& rhi)
	{
		m_RhiBuffer.reset(rhi.CreateBuffer());
	}

}

