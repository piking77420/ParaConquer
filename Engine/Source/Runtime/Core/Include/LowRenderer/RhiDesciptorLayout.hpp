#pragma once
#include "RhiObject.hpp"
#include "RhiDescriptorBindings.hpp"

namespace PC_CORE::RHI 
{
	class PC_CORE_API RhiDescriptorLayout : public RhiObjectT<RhiDescriptorLayout>
	{
	public:


		RhiDescriptorLayout(Rhi& _Rhi);

		~RhiDescriptorLayout() override;


	private:
		
	};


}