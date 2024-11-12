#include "rendering/render_harware_interface/descriptor_set.hpp"

#include "rendering/render_harware_interface/RHI.hpp"

void PC_CORE::UpdateDescriptorSet(DescriptorWriteSet* _descriptorWrites, uint32_t _descritptorSetWriteCount)
{
    RHI::GetInstance().UpdateDescriptorSet(_descritptorSetWriteCount, _descriptorWrites);
}


