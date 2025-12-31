#pragma once

#include <variant>

#include "RhiTypedef.h"

namespace PC_CORE
{
	class CommandList;
	class RhiTexture;
	class RhiBuffer;
}

namespace PC_CORE::RHI
{
	namespace ResourceUpdate {
		enum ResourceUpdateStatus
		{
			Failed,
			Success,
			Complete
		};

		class PC_CORE_API BufferUpload
		{
		public:
			explicit BufferUpload(RhiBuffer& _RhiBuffer, const void* _Data, size_t _Size);

			~BufferUpload() = default;

			DEFAULT_COPY_MOVE_OPERATIONS(BufferUpload)

			[[nodiscard]] ResourceUpdateStatus Execute(CommandList& _CommandList);

		private:
			RhiBuffer* m_RhiBuffer;

			size_t m_NbrOfUpdate{ 0u };

			std::unique_ptr<uint8_t[]> m_Data;

			size_t m_DataSize{ 0u };
		};
	}
	

	class PC_CORE_API ResourceUpdateBranch
	{
	public:
		DEFAULT_CONSTRUCTOR_DESTRUCTOR(ResourceUpdateBranch)

		DEFAULT_COPY_MOVE_OPERATIONS(ResourceUpdateBranch)

	    void BufferUpload(RhiBuffer& _RhiBuffer, const void* _Data, size_t _Size);

		void Reset()
		{
			m_UpdateBranch = std::monostate();
		}

		ResourceUpdate::ResourceUpdateStatus Execute(CommandList& _CommandList);
	private:
		std::variant<std::monostate, ResourceUpdate::BufferUpload> m_UpdateBranch;
	};


}