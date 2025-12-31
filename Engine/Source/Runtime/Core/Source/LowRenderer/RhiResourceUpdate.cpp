#include "LowRenderer/RhiResourceUpdate.hpp"

#include "LowRenderer/CommandList.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiBuffer.h"

template<class... Ts>
struct Overload : Ts... { using Ts::operator()...; };


namespace PC_CORE::RHI	
{

namespace ResourceUpdate 
{

	BufferUpload::BufferUpload(RhiBuffer& _RhiBuffer, const void* _Data, size_t _Size)
		: m_RhiBuffer(&_RhiBuffer)
		, m_NbrOfUpdate(m_RhiBuffer->GetNbrOfResourcePerFrameInFlight())
		, m_DataSize(_Size)
	{

		assert(m_DataSize != 0);
		m_Data = std::make_unique<uint8_t[]>(m_DataSize);
		std::memcpy(m_Data.get(), _Data, m_DataSize);
	}

	ResourceUpdateStatus BufferUpload::Execute(CommandList& _CommandList)

	{
		assert(m_Data && m_DataSize != 0);

		if (!m_RhiBuffer->UploadData(&_CommandList, m_Data.get(), m_DataSize))
			return ResourceUpdateStatus::Failed;

		m_NbrOfUpdate--;
		return m_NbrOfUpdate > 0 ? ResourceUpdateStatus::Success : ResourceUpdateStatus::Complete;
	}

}

void ResourceUpdateBranch::BufferUpload(RhiBuffer& _RhiBuffer, const void* _Data, size_t _Size)
{
	assert(m_UpdateBranch.index() == 0);

	m_UpdateBranch.emplace<ResourceUpdate::BufferUpload>(_RhiBuffer, _Data, _Size);
}

ResourceUpdate::ResourceUpdateStatus ResourceUpdateBranch::Execute(CommandList& _CommandList)
{
	return std::visit(
		Overload{
			[&](std::monostate&) {
				return ResourceUpdate::Failed;
			},
			[&](ResourceUpdate::BufferUpload& upload) {
				return upload.Execute(_CommandList);
			},
			
		},
		m_UpdateBranch
	);
}

}
