#ifndef PC_RHI_RHICOMPUTE_PIPELINE
#define PC_RHI_RHICOMPUTE_PIPELINE

#include <LowRenderer/RhiPipeline.hpp>

namespace PC_CORE
{
	class RhiComputePipeline : public RhiPipeline
	{
	public:
		RhiComputePipeline(Rhi& _Rhi);
		virtual ~RhiComputePipeline();

	private:

	};

} // namespace PC_CORE


#endif // !PC_RHI_RHICOMPUTE_PIPELINE
