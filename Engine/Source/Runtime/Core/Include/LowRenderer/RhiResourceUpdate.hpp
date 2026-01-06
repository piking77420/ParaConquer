#pragma once

#include <vector>
#include <variant>
#include <deque>

#include "RhiResource.hpp"

namespace PC_CORE
{
	class CommandList;
	class RhiTexture;
	class RhiBuffer;
}

namespace PC_CORE::RHI
{
	namespace ResourceUpdateOperation {
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
			RhiBuffer* m_RhiBuffer{ nullptr };

			size_t m_NbrOfUpdate{ 0u };

			std::unique_ptr<uint8_t[]> m_Data;

			size_t m_DataSize{ 0u };
		};

		class PC_CORE_API TextureUpload2D
		{
		public:
			explicit TextureUpload2D(RhiTexture& _RhiTexture, const void* _Data, RhiFormat _Format,
				size_t _ImageWidht, size_t _ImageHeight, RhiResourceState _AfterUploadState);

			~TextureUpload2D() = default;

			DEFAULT_COPY_MOVE_OPERATIONS(TextureUpload2D)

			[[nodiscard]] ResourceUpdateStatus Execute(CommandList& _CommandList);

		private:
			RhiTexture* m_RhiTexture{ nullptr };

			size_t m_NbrOfUpdate{ 0u };

			std::unique_ptr<uint8_t[]> m_Data;

			size_t m_ImageWidht{ 0u };

			size_t m_ImageHeight{ 0u };

			RhiFormat m_RhiFormat{ RhiFormat::Undefined };

			RhiResourceState m_AfterUploadState{ RhiResourceState::Undefined };

		};

		
		class PC_CORE_API GenerateMipMap
		{
		public:
			explicit GenerateMipMap(RhiTexture& _RhiTexture);

			~GenerateMipMap() = default;

			DEFAULT_COPY_MOVE_OPERATIONS(GenerateMipMap)

			[[nodiscard]] ResourceUpdateStatus Execute(CommandList& _CommandList);

		private:
			RhiTexture* m_RhiTexture{ nullptr };

			size_t m_NbrOfUpdate{ 0u };
		};

	}

	class PC_CORE_API ResourceUpdateBranch
	{
	public:
		using ResourceUpdate = std::variant<std::monostate, ResourceUpdateOperation::BufferUpload, ResourceUpdateOperation::TextureUpload2D>;

		DEFAULT_CONSTRUCTOR_DESTRUCTOR(ResourceUpdateBranch)

		ResourceUpdateBranch(const ResourceUpdateBranch&) = delete;
		ResourceUpdateBranch& operator=(const ResourceUpdateBranch&) = delete;

		ResourceUpdateBranch(ResourceUpdateBranch&&) = default;
		ResourceUpdateBranch& operator=(ResourceUpdateBranch&&) = default;

		ResourceUpdateBranch& BufferUpload(RhiBuffer& _RhiBuffer, const void* _Data, size_t _Size);

		ResourceUpdateBranch& TextureUpload2D(RhiTexture& _RhiTexture, const void* _Data, RhiFormat _Format, 
											 size_t _ImageWidht, size_t _ImageHeight, RhiResourceState _AfterUploadState);

		bool IsEmpty() const
		{
			return m_UpdateBranchs.empty();
		}

		bool Proceed(CommandList& _CommandList);

	private:
		ResourceUpdateOperation::ResourceUpdateStatus Execute(CommandList& _CommandList, ResourceUpdate& _ResourceUpdate);

		std::deque<ResourceUpdate> m_UpdateBranchs;
	};


}