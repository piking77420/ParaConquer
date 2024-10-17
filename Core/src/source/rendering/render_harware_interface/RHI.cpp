#include "rendering/render_harware_interface/RHI.hpp"
#include "log.hpp"

using namespace PC_CORE;

RHI* RHI::m_Instance = nullptr;

void RHI::MakeInstance(RHI* _rhi)
{
	if (m_Instance != nullptr)
	{
		PC_LOGERROR("MakeInstance rhi m_Instance is already defined");
		delete _rhi;
		return;
	}
	m_Instance = _rhi;
}

void RHI::DestroyInstance()
{
	delete m_Instance;
	m_Instance = nullptr;
}

RHI* const RHI::GetInstance()
{
	return m_Instance;
}

