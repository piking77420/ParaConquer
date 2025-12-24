#include "Rendering/Buffer/VertexBuffer.hpp"

#include "LowRenderer/Rhi.hpp"

namespace PC_CORE
{

VertexBuffer::VertexBuffer(PC_CORE::Rhi& rhi)
{
	m_RhiBuffer.reset(rhi.CreateBuffer());
}

}

