#pragma once

#include "core_header.hpp"
#include "rhi_render_pass.hpp"
#include "math/matrix3x3.hpp"
#include "math/toolbox_typedef.hpp"
#include "resources/texture.hpp"

BEGIN_PCCORE
	struct CreateFrameInfo
{
	uint32_t width;
	uint32_t height;

	std::vector<Texture*>* attachements;
	RhiRenderPass* renderPass;
};

class FrameBuffer
{
public:

	PC_CORE_API FrameBuffer(const CreateFrameInfo& _createFrameInfo);

	PC_CORE_API FrameBuffer() = default;

	PC_CORE_API virtual ~FrameBuffer() = default;

	PC_CORE_API uint32_t GetWidth() const
	{
		return m_Width;
	}

	PC_CORE_API uint32_t GetHeight() const
	{
		return m_Height;
	}
	
	PC_CORE_API Tbx::Vector2ui Size() const
	{
		return {m_Width, m_Height};
	}

protected:
	uint32_t m_Width;

	uint32_t m_Height;
};

END_PCCORE