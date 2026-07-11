#ifndef PC_RHI_RHICOMPUTE_PIPELINE
#define PC_RHI_RHICOMPUTE_PIPELINE

#include <LowRenderer/RhiPipeline.hpp>

namespace PC_CORE
{
	class RhiComputePipeline : public RhiPipeline
	{
	public:
		struct LocalSize
		{
			uint32_t x{ 0u };
			uint32_t y{ 0u };
			uint32_t z{ 0u };
		};

		RhiComputePipeline(Rhi& _Rhi);
		virtual ~RhiComputePipeline();

		const LocalSize& GetLocalSize() const
		{
			return m_LocalSize;
		}

	protected:
		LocalSize m_LocalSize{};
	};

} // namespace PC_CORE


#endif // !PC_RHI_RHICOMPUTE_PIPELINE
