#pragma once

#include "CoreHeader.hpp"

#include "RhiRenderPass.hpp"
#include "Math/Matrix3x3.hpp"
#include "Math/ToolboxTypedef.hpp"
#include "Resources/Texture2d.hpp"

BEGIN_PCCORE
	struct FrameBufferAttachementDesriptor
{
	Texture2D* texture;
};

struct CreateFrameInfo
{
	uint32_t width;
	uint32_t height;

	std::vector<FrameBufferAttachementDesriptor>* attachements;
	RhiRenderPass* renderPass;
};

class FrameBuffer : public RhiResource
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